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
#pragma once

#include "config.h"
#include "types.h"

#include <core/dbus-application.h>

#include <vector>

enum class gpio_set_result
{
    success,
    no_change,
    name_not_found,
    gpiod_error,
};

class application : public core::dbus_application
{
public:
    explicit application(configuration const& config);
    ~application();

    application(application const&) = delete;
    application& operator=(application const&) = delete;

protected:
    void pre_run() final;
    void post_run() final;

private:
    void setup_gpio();
    void setup_dbus_interface();

    static int gdc_get_property_lines(sd_bus*, const char*, const char*, const char*,
                                      sd_bus_message *reply, void *userdata, sd_bus_error *ret_error);
    int dbus_property_get_lines(sd_bus_message *reply, sd_bus_error *ret_error);

    static int gdc_set_line_handler(sd_bus_message *m, void *userdata, sd_bus_error *ret_error);
    int dbus_set_line_handler(sd_bus_message* msg, sd_bus_error* ret_error);
    gpio_set_result set_line(std::string const& name, gpio::level lev);

private:
    configuration _config;
    std::vector<gpio::gpio_line> _gpios{};

    sd_bus_slot* _vtable_slot{nullptr};
};