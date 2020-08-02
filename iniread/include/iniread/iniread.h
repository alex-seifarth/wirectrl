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

#include <string>
#include <vector>
#include <ostream>
#include <istream>

namespace iniread {

    struct property
    {
        std::string name{};
        std::string value{};
        int line_number;
    };

    struct section
    {
        std::string name{};
        std::string value{};
        std::vector<section> subsections{};
        std::vector<property> properties{};
        int line_number{0};
    };

    section read_ini_file(std::istream& input);
    void write_init_file(std::ostream& output);
}