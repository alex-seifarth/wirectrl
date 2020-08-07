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
#include <core/dbus-application.h>
#include <core/exception.h>
#include <core/final.h>

using namespace core;

dbus_application::dbus_application(DBusType dbus_type_, std::string connection_name_)
    : core::application{}
    , _dbus_type{dbus_type_}
    , _connection_name{std::move(connection_name_)}
{
    int r;
    if (_dbus_type == DBusType::Session) {
        r = sd_bus_open_user(&_sd_bus);
    }
    else {
        r = sd_bus_open_system(&_sd_bus);
    }
    if (r < 0) {
        throw core::runtime_exception{"cannot get system d-bus", r};
    }
    core::final unref_bus{[this](){sd_bus_unref(_sd_bus);}};

    if (!_connection_name.empty()) {
        r = sd_bus_request_name(_sd_bus, _connection_name.c_str(), 0);
        if (r < 0) {
            throw core::runtime_exception{"cannot get name on d-bus", r};
        }
    }

    unref_bus.reset();
}

void dbus_application::run()
{
    int r = sd_bus_attach_event(_sd_bus, get_sd_event().get(), 0);
    if (r < 0) {
        throw core::runtime_exception{"unable to attach dbus to event loop", r};
    }
    application::run();
}

dbus_application::~dbus_application()
{
    sd_bus_unref(_sd_bus);
}
