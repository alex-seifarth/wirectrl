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
// along with this program.  If not, see <https://www.gnu.org/licenses/>
#include "types.h"

#include <core/final.h>

#include <stdexcept>
#include <cerrno>

using namespace gpio;

gpio_line::gpio_line(std::string name, std::string chip, unsigned line,
                     std::string const& consumer, gpio::level init_level,
                     active_level al)
    : _name{std::move(name)}
{
    _chip = gpiod_chip_open_lookup(chip.c_str());
    if (!_chip) {
        throw gpio_exception{"chip not found", errno};
    }
    core::final close_chip{[pchip = _chip](){gpiod_chip_close(pchip);}};

    _line = gpiod_chip_get_line(_chip, line);
    if (!_line) {
        throw gpio_exception{"line cannot be reserved", 0};
    }
    close_chip.reset([pline = _line](){gpiod_line_close_chip(pline);});

    gpiod_line_request_config lrc {consumer.c_str(), GPIOD_LINE_REQUEST_DIRECTION_OUTPUT,
        al == active_level::active_low ? GPIOD_LINE_REQUEST_FLAG_ACTIVE_LOW : 0};
    if (0 != gpiod_line_request(_line, &lrc, init_level == level::inactive ? 0 : 1)) {
        throw gpio_exception{"cannot reserve requested line", 0};
    }
    close_chip.reset();
}

gpio_line::gpio_line(gpio_line&& old) noexcept
    : _name{std::move(old._name)}
    , _chip{old._chip}
    , _line{old._line}
{
    old._name.clear();
    old._chip = nullptr;
    old._line = nullptr;
}

gpio_line::~gpio_line()
{
    if (_line) {
        gpiod_line_close_chip(_line);
    }
}

// ----------------------------------------------------------------------------
// gpio_exception
// ----------------------------------------------------------------------------
gpio_exception::gpio_exception(std::string msg, int error)
    : _message{std::move(msg)}
    , _errorno{error}
{}

std::string const& gpio_exception::message() const noexcept
{
    return _message;
}

