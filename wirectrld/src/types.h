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

#include <gpiod.h>

#include <string>

namespace gpio {
    enum class level {
        active,
        inactive,
    };

    enum class active_level {
        undefined,
        active_low,
        active_high,
    };

    enum class pull_resistor {
        none,
        up,
        down,
    };

    class gpio_line
    {
    public:
        gpio_line(std::string name, std::string chip, unsigned line,
                  std::string const& consumer, gpio::level init_level, active_level al);
        ~gpio_line();

        gpio_line(gpio_line&&) noexcept;

        std::string const& name() const;

        gpio::level level() const;

        //! @throw  gpio_exception   Thrown when GPIOD returns an error
        //! @return Returns true if level has changed, false if level stays the same.
        bool set_level(gpio::level lev);

    private:
        std::string _name;
        gpiod_chip *_chip;
        gpiod_line *_line;
        gpio::level _level;
    };

    class gpio_exception : public std::exception
    {
    public:
        explicit gpio_exception(std::string msg, int error);

        std::string const& message() const noexcept;

        int error() const;

    private:
        std::string _message;
        int _errorno;
    };

} // namespace gpio

