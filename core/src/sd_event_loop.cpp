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
#include <core/sd_event_loop.h>
#include <core/exception.h>

#include <systemd/sd-event.h>

using namespace core;

void core::swap(core::sd_event_loop& rhs, core::sd_event_loop& lhs) noexcept
{
    using std::swap;
    swap(rhs._sd_event, lhs._sd_event);
}

core::sd_event_loop::sd_event_loop(sd_event* sd_event_)
    : _sd_event{sd_event_}
{}

core::sd_event_loop::~sd_event_loop()
{
    sd_event_unref(_sd_event);
    _sd_event = nullptr;
}

core::sd_event_loop::sd_event_loop(core::sd_event_loop const& rhs) noexcept
    : _sd_event{rhs._sd_event}
{
    sd_event_ref(_sd_event);
}

core::sd_event_loop& core::sd_event_loop::operator=(core::sd_event_loop lhs) noexcept
{
    swap(*this, lhs);
    return *this;
}

sd_event* core::sd_event_loop::get() const noexcept
{
    return _sd_event;
}

core::sd_event_loop core::sd_event_loop::default_loop()
{
    sd_event *def_event{nullptr};
    int r = sd_event_default(&def_event);
    if (r < 0) {
        throw core::runtime_exception{"unable to allocate default event loop", r};
    }
    return sd_event_loop{def_event};
}

core::sd_event_loop core::sd_event_loop::create()
{
    sd_event* ev;
    int r = sd_event_new(&ev);
    if (r < 0) {
        throw core::runtime_exception{"unable to allocate event loop", r};
    }
    return sd_event_loop{ev};
}
