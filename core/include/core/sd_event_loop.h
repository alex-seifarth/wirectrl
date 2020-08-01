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

#include <stdexcept>

struct sd_event;

namespace core {

    class sd_event_loop
    {
    public:
        ~sd_event_loop();
        sd_event_loop(sd_event_loop const& rhs) noexcept;
        sd_event_loop& operator=(sd_event_loop lhs) noexcept;

        friend void swap(sd_event_loop& rhs, sd_event_loop& lhs) noexcept;

        sd_event* get() const noexcept;

        static sd_event_loop create();
        static sd_event_loop default_loop();

    private:
        explicit sd_event_loop(sd_event*);
        sd_event* _sd_event;
    };

    void swap(sd_event_loop& rhs, sd_event_loop& lhs) noexcept;
}