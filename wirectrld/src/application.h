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

private:
    configuration _config;
    std::vector<gpio::gpio_line> _gpios{};
};