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

#include <core/application.h>
#include <systemd/sd-bus.h>
#include <string>

namespace core {

    enum class DBusType {
        System,
        Session,
    };

    class dbus_application : public core::application
    {
    public:
        explicit dbus_application(DBusType dbus_type_, std::string connection_name = std::string{});
        ~dbus_application();

        dbus_application(dbus_application const&) = delete;
        dbus_application& operator=(dbus_application const&) = delete;

        void run() final; //! \todo final vs. override

    protected:
        void pre_run() override {};
        void post_run() override {};

        sd_bus* bus() const;

    private:
        DBusType _dbus_type;
        sd_bus *_sd_bus{nullptr};
        std::string _connection_name;
    };

}