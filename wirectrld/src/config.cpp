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
#include "config.h"

#include <algorithm>
#include <cstring>
#include <fstream>
#include <regex>
#include <stdexcept>
#include <string>

#define ENVVAR_NAME_CONFIG      ("WIRECTRL_CONFIG_FILE")
#define ETC_PATH_CONFIG         ("/etc/wirectrl/wirectrl.conf")
#define HOME_PATH_CONFIG        ("/.wirectrl/wirectrl.conf")
#define ENVVAR_HOME             ("HOME")

namespace {

bool read_config_file(std::string const& path, core::ini::file &ini_file)
{
    std::ifstream f(path);
    if (!f.is_open()) {
        return false;
    }
    try {
        ini_file = core::ini::file{f};
    }
    catch(std::runtime_error& e) {
        throw std::runtime_error{std::string{e.what()} + " [" + path + "]"};
    }
    return true;
}

std::string get_prop_value(core::ini::section const& section, std::string const& prop_name, std::string const& def_value)
{
    auto it = std::find_if(section.properties.cbegin(), section.properties.cend(),
                   [&prop_name](core::ini::property const& p){return p.name == prop_name;});
    if (it == section.properties.cend()) {
        return def_value;
    }
    return it->value;
}

template<typename T>
T get_prop_value_enum(core::ini::section const& section, std::string const& prop_name,
                      std::vector<std::pair<std::string, T>> const& values, T const& def_value = T{})
{
    auto it = std::find_if(section.properties.cbegin(), section.properties.cend(),
                           [&prop_name](core::ini::property const& p){return p.name == prop_name;});
    if (it == section.properties.cend()) {
        return def_value;
    }
    auto rit = std::find_if(values.cbegin(), values.cend(),
                    [str = it->value](std::pair<std::string, T> const& v){return v.first == str;});
    if (rit == values.cend()) {
        return def_value;
    }
    return rit->second;
}

} // namespace

std::pair<core::ini::file, std::string> get_config(opts const& options)
{
    core::ini::file ini_file;

    if (!options.config_file.empty()) {
        if (!read_config_file(options.config_file, ini_file)) {
            std::string msg{"Cannot open configuration file from command line argument: "};
            msg += options.config_file;
            throw std::runtime_error{msg};
        }
        return std::make_pair(ini_file, std::string{"-c "} + options.config_file);
    }

    auto env_var_wirectrl_config = getenv(ENVVAR_NAME_CONFIG);
    if (env_var_wirectrl_config != nullptr && strlen(env_var_wirectrl_config) > 0) {
        if (!read_config_file(env_var_wirectrl_config, ini_file)) {
            std::string msg{"Cannot open configuration file from environment variable: "};
            msg += env_var_wirectrl_config;
            throw std::runtime_error{msg};
        }
        return std::make_pair(ini_file, std::string{ENVVAR_NAME_CONFIG} + "=" + std::string{env_var_wirectrl_config});
    }

    auto env_var_home = getenv(ENVVAR_HOME);
    if (env_var_home != nullptr && strlen(env_var_home) > 0) {
        std::string path{env_var_home};
        path += HOME_PATH_CONFIG;
        if (read_config_file(path, ini_file)) {
            return std::make_pair(ini_file, path);
        }
    }
    if (read_config_file(ETC_PATH_CONFIG, ini_file)) {
        return std::make_pair(ini_file, std::string{ETC_PATH_CONFIG});
    }
    throw std::runtime_error{"No configuration file found."};
}

configuration configuration::decode_from_section(core::ini::file const& ini_file)
{
    configuration c;

    bool dbus_section_found {false};
    for (auto const& section : ini_file.sections()) {
        if (section.name == "dbus") {
            if (dbus_section_found) {
                // more than one dbus section
                throw std::runtime_error{"Multiple 'dbus' sections in configuration file."};
            }
            dbus_section_found = true;
            c.dbus = dbus_configuration::decode_from_section(section);
        }
        else if (section.name == "gpio") {
            c.gpios.push_back(gpio_configuration::decode_from_section(section));
        }
        else {
            throw std::runtime_error{std::string{"Unknown section type: "} + section.name};
        }
    }
    return c;
}

dbus_configuration dbus_configuration::decode_from_section(core::ini::section const& section)
{
    dbus_configuration dc;
    dc.connection_name = get_prop_value(section, "connection-id", "de.titnc.pi.wirectrl");
    dc.object_name = get_prop_value(section, "object-id", "/de/titnc/pi/wirectrl/v1");
    auto str_use_session_bus = get_prop_value(section, "use-session-bus", "true");
    dc.use_session_bus = (str_use_session_bus == "true");

    // sanity checks
    std::regex connection_name_regex{R"(([a-z0-9_]+)(\.[a-z0-9_]+)*)"};
    if (!std::regex_match(dc.connection_name, connection_name_regex)) {
        throw std::runtime_error{std::string{"DBus connection name invalid:"} + dc.connection_name};
    }
    std::regex object_name_regex{R"((/[a-z0-9_]+)(/[a-z0-9_]+)*)"};
    if (!std::regex_match(dc.object_name, object_name_regex)) {
        throw std::runtime_error{std::string{"DBus object name invalid:"} + dc.object_name};
    }
    std::regex use_session_bus_regex{R"(true|false)"};
    if (!str_use_session_bus.empty() && !std::regex_match(str_use_session_bus, use_session_bus_regex)) {
        throw std::runtime_error{std::string{"Invalid value for dbus.use-session-bus :"} + str_use_session_bus};
    }
    return dc;
}

gpio_configuration gpio_configuration::decode_from_section(core::ini::section const& section)
{
    gpio_configuration gc;
    gc.name = get_prop_value(section, "name", std::string{});
    gc.consumer = get_prop_value(section, "consumer", "wirectrl");

    gc.active_level = get_prop_value_enum<gpio::active_level>(section, "active-level",
           {{"low", gpio::active_level::active_low}, {"high", gpio::active_level::active_high}},
           gpio::active_level::undefined);

    return gc;
}
