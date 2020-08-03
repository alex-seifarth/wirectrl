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

#include "opts.h"
#include <iniread/iniread.h>
#include <tuple>

//! Searches for the configuration file and parses it.
//! The method searches for the configuration file in the following order:
//! 1. If a path is given in options.config_file (from command line -c) this file is parsed.
//!    If the file does not exist the function throws a std::runtime error.
//! 2. If the environment variable WIRECTRL_CONFIG_FILE is set it is interpreted as config file path
//!    If the file does not exist the function throws a std::runtime error.
//! 3. If the file $HOME/.wirectrl/wirectrl.conf exist it is parsed as configuration file.
//!    If the $HOME environment variable is not set, it is the same as when the file does not exist.
//! 4. If the file /etc/wirectrl/wirectrl.conf exists it is parsed as configuration file.
//! 5. When no configuration file is found or an existing file is parsed with ini-errors, the function
//!    throws a std::runtime_error.
//! @throws     std::runtime_error  Thrown when expected files are not found or ini-syntax errors occurred.
//! @return     Returns a pair consisting of the ini-file root section and the ini-file's path
std::pair<iniread::section, std::string> get_config(opts const& options);
