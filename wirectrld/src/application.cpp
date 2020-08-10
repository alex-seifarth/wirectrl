// wirectrl is a daemon for systemd to control GPIO ports of raspberry pi
// Copyright (C) 2020 Alexander Seifarth
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
#include "application.h"

#include <systemd/sd-journal.h>

#include <cassert>
#include <cstring>
#include <algorithm>

// ----------------------------------------------------------------------------
// application
// ----------------------------------------------------------------------------
application::application(configuration const& config)
    : core::dbus_application{config.dbus.use_session_bus ? core::DBusType::Session : core::DBusType::System,
                             config.dbus.connection_name}
    , _config{config}
{}

application::~application() = default;

void application::pre_run()
{
    setup_gpio();
    setup_dbus_interface();
}

void application::post_run()
{
    sd_bus_slot_unref(_vtable_slot);
    _vtable_slot = nullptr;
    _gpios.clear();
}

void application::setup_gpio()
{
    for (auto const& g : _config.gpios) {
        try {
            _gpios.emplace_back(g.name, g.gpio_chip_name, g.gpio_line_id,
                                g.consumer, g.initial_level, g.active_level);
        }
        catch(gpio::gpio_exception& e) {
            sd_journal_print(LOG_ERR, "GPIO line setup failed %s (%s-%i): %s", g.name.c_str(),
                             g.gpio_chip_name.c_str(), g.gpio_line_id, e.message().c_str());

        }
    }
}

int application::gdc_get_property_lines(sd_bus */*bus*/, const char */*path*/,
                            const char */*interface*/,
                            const char */*property*/,
                            sd_bus_message *reply,
                            void *userdata,
                            sd_bus_error *ret_error)
{
    assert(userdata != nullptr);
    auto app = reinterpret_cast<application*>(userdata);
    return app->dbus_property_get_lines(reply, ret_error);
}

void application::setup_dbus_interface()
{
#define WIRECTRL_INTERFACE          ("de.titnc.pi.wirectrl")
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
    static const sd_bus_vtable _vtable[] = {
            SD_BUS_VTABLE_START(0),
            SD_BUS_PROPERTY("lines", "a(si)", &application::gdc_get_property_lines,  0, SD_BUS_VTABLE_PROPERTY_EMITS_CHANGE),
            SD_BUS_METHOD("set_line", "si", "i", &application::gdc_set_line_handler, SD_BUS_VTABLE_UNPRIVILEGED),
            SD_BUS_VTABLE_END
    };
#pragma GCC diagnostic pop

    auto r = sd_bus_add_object_vtable(dbus_application::bus(),  &_vtable_slot,
                                 _config.dbus.object_name.c_str(),
                                 WIRECTRL_INTERFACE,
                                 _vtable,
                                 this);
    if (r < 0) {
        sd_journal_print(LOG_ERR, "Unable to register DBus interface for wirectrl (%s)", strerror(-r));
        throw std::runtime_error{"Unable to register DBus interface"};
    }
}

namespace {

    int encode_gpio_lines(sd_bus_message *msg, std::vector<gpio::gpio_line> const& gpios)
    {
        int r = sd_bus_message_open_container(msg, 'a', "(si)");
        if (r < 0) {
            return r;
        }
        for (auto const& gpio : gpios) {
            r = sd_bus_message_append(msg, "(si)", gpio.name().c_str(), gpio.level() == gpio::level::active ? 1 : 0);
            if (r < 0) {
                return r;
            }
        }
        r = sd_bus_message_close_container(msg);
        if (r < 0) {
            return r;
        }
        return 1;
    }

} // namespace

int application::dbus_property_get_lines(sd_bus_message *reply, sd_bus_error */*ret_error*/)
{
    auto r = encode_gpio_lines(reply, _gpios);
    if (r < 0) {
        sd_journal_print(LOG_ERR, "Unable to fill property 'line' message (%s, %i)",
                         strerror(-r), -r);
    }
    return r;
}

int application::gdc_set_line_handler(sd_bus_message *m, void *userdata, sd_bus_error *ret_error)
{
    assert(userdata != nullptr);
    auto app = reinterpret_cast<application*>(userdata);
    return app->dbus_set_line_handler(m, ret_error);
}

int application::dbus_set_line_handler(sd_bus_message* msg, sd_bus_error* ret_error)
{
    (void)msg;
    (void)ret_error;

    char const* line_name{nullptr};
    int line_level{-1};
    int r;
    r = sd_bus_message_read(msg, "si", &line_name, &line_level);
    if (r < 0) {
        sd_journal_print(LOG_WARNING, "Client request 'set_line' with invalid arguments (%i, %s)",
                         -r, strerror(-r));
        return r;
    }

    if (line_level != 0 && line_level != 1) {
        sd_bus_error_set_const(ret_error, "de.titnc.pi.wirectrl:set_line", "Invalid value for line level, must be 0|1");
        return -EINVAL;
    }
    if (line_name == nullptr || std::strlen(line_name) == 0) {
        sd_bus_error_set_const(ret_error, "de.titnc.pi.wirectrl:set_line", "Invalid value for line name");
        return -EINVAL;
    }

    auto result = set_line(line_name, line_level == 0 ? gpio::level::inactive : gpio::level::active);
    switch (result) {
        case gpio_set_result::success:
            sd_bus_emit_properties_changed(dbus_application::bus(),
                                           _config.dbus.object_name.c_str(),
                                           WIRECTRL_INTERFACE,
                                           "lines",
                                           nullptr);
            return sd_bus_reply_method_return(msg, "i", 0);
        case gpio_set_result::no_change:
            return sd_bus_reply_method_return(msg, "i", 1);
            break;
        case gpio_set_result::name_not_found:
            sd_bus_error_set_const(ret_error, "LineNameNotFound", "Line name is not configured or failed at setup");
            return -EINVAL;
        case gpio_set_result::gpiod_error:
            sd_bus_error_set_const(ret_error, "GpiodError", "LibGpiod reported error");
            return -EINVAL;
    }
    return -EINVAL;
}

gpio_set_result application::set_line(std::string const& name, gpio::level lev)
{
    auto it = std::find_if(_gpios.begin(), _gpios.end(),
                           [&name](gpio::gpio_line const& l){return l.name() == name;});
    if (it == _gpios.end()) {
        return gpio_set_result::name_not_found;
    }

    try {
        if (!it->set_level(lev)) {
            return gpio_set_result::no_change;
        }
        return gpio_set_result::success;
    }
    catch(gpio::gpio_exception& e) {
        sd_journal_print(LOG_ERR, "GPIOD exception while setting line level. (%s, %i, %s)",
                         e.message().c_str(), e.error(), strerror(e.error()));
        return gpio_set_result::gpiod_error;
    }
}


