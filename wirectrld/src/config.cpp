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
#include <iniread/iniread.h>

#include <cstring>
#include <fstream>
#include <stdexcept>

#define ENVVAR_NAME_CONFIG      ("WIRECTRL_CONFIG_FILE")
#define ETC_PATH_CONFIG         ("/etc/wirectrl/wirectrl.conf")
#define HOME_PATH_CONFIG        ("/.wirectrl/wirectrl.conf")
#define ENVVAR_HOME             ("HOME")

namespace {

bool read_config_file(std::string const& path, iniread::section& root_section)
{
    std::ifstream f(path);
    if (!f.is_open()) {
        return false;
    }
    try {
        root_section = iniread::read_ini_file(f);
    }
    catch(std::runtime_error& e) {
        throw std::runtime_error{std::string{e.what()} + " [" + path + "]"};
    }
    return true;
}

} // namespace

std::pair<iniread::section, std::string> get_config(opts const& options)
{
    iniread::section root_section;

    if (!options.config_file.empty()) {
        if (!read_config_file(options.config_file, root_section)) {
            std::string msg{"Cannot open configuration file from command line argument: "};
            msg += options.config_file;
            throw std::runtime_error{msg};
        }
        return std::make_pair(root_section, std::string{"-c "} + options.config_file);
    }

    auto env_var_wirectrl_config = getenv(ENVVAR_NAME_CONFIG);
    if (env_var_wirectrl_config != nullptr && strlen(env_var_wirectrl_config) > 0) {
        if (!read_config_file(env_var_wirectrl_config, root_section)) {
            std::string msg{"Cannot open configuration file from environment variable: "};
            msg += env_var_wirectrl_config;
            throw std::runtime_error{msg};
        }
        return std::make_pair(root_section, std::string{ENVVAR_NAME_CONFIG} + "=" + std::string{env_var_wirectrl_config});
    }

    auto env_var_home = getenv(ENVVAR_HOME);
    if (env_var_home != nullptr && strlen(env_var_home) > 0) {
        std::string path{env_var_home};
        path += HOME_PATH_CONFIG;
        if (read_config_file(path, root_section)) {
            return std::make_pair(root_section, path);
        }
    }
    if (read_config_file(ETC_PATH_CONFIG, root_section)) {
        return std::make_pair(root_section, std::string{ETC_PATH_CONFIG});
    }
    throw std::runtime_error{"No configuration file found."};
}