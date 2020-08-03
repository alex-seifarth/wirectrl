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
#include "opts.h"
#include <unistd.h>

#include <cstdlib>
#include <iostream>
#include <stdexcept>

opts parse_program_options(int argc, char * const argv[])
{
    static const char* opt_string = "c:";

    opts options{};
    int opt;

    while ((opt = getopt(argc, argv, opt_string)) != -1 ) {
        switch (opt) {
            case 'c':
                options.config_file = std::string{optarg};
                break;
            case '?':
                std::string error{"Unknown option: "};
                error += static_cast<char>(opt);
                throw std::runtime_error{error};
        }
    }
    return options;
}

