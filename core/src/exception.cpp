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
#include <core/exception.h>

#include <sstream>

using namespace core;

runtime_exception::runtime_exception(std::string msg, int error_code)
    : std::runtime_error{msg}
    , _error_code{error_code}
{}

std::string runtime_exception::message() const
{
    std::stringstream ss;
    ss << std::runtime_error::what();
    if (_error_code != 0) {
        ss << "(" << _error_code << ")";
    }
    return ss.str();
}
