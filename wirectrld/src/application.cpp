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

void application::setup_dbus_interface()
{
}
