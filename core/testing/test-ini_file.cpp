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

#include <doctest/doctest.h>
#include <core/ini.h>

#include <sstream>

TEST_CASE("ini-file empty")
{
    core::ini::file f{};
    CHECK(f.sections().empty());
}

TEST_CASE("ini-file from stream")
{
    const char* txt =
R"ini(
[dbus]
connection-id = "de.titnc.pi.wirectrl"
object-id = /de/titnc/pi/wirectrl/v1
use-session-bus = false

[gpio = 0-17]
name = "AV-Receiver"
consumer = "asdd" #default is wirectrl
idle-value = off
on-error = ignore|terminate
active-level = high|low
pull-resistor =
)ini";

    std::istringstream istr{txt};
    core::ini::file f{istr};

    REQUIRE_EQ(f.sections().size(), 2);

    auto const& dbus_section = f.sections()[0];
    CHECK_EQ(dbus_section.name, "dbus");
    CHECK(dbus_section.value.empty());
    REQUIRE_EQ(dbus_section.properties.size(), 3);
    CHECK_EQ(dbus_section.properties[0].name, "connection-id");
    CHECK_EQ(dbus_section.properties[0].value, "de.titnc.pi.wirectrl");
    CHECK_EQ(dbus_section.properties[1].name, "object-id");
    CHECK_EQ(dbus_section.properties[1].value, "/de/titnc/pi/wirectrl/v1");
    CHECK_EQ(dbus_section.properties[2].name, "use-session-bus");
    CHECK_EQ(dbus_section.properties[2].value, "false");

    auto const& gpio_section = f.sections()[1];
    CHECK_EQ(gpio_section.name, "gpio");
    CHECK_EQ(gpio_section.value, "0-17");
    CHECK_EQ(gpio_section.properties.size(), 6);
    CHECK_EQ(gpio_section.properties[5].name, "pull-resistor");
    CHECK_EQ(gpio_section.properties[5].value, std::string{});
}