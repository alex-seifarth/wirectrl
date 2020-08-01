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
#include <core/application.h>
#include <core/final.h>
#include "sig_set_ctrl.h"

#include <systemd/sd-event.h>

#include <cassert>

using namespace core;

application::application()
    : _sd_event_loop{sd_event_loop::default_loop()}
{
    assert(_sd_event_loop.get());

}

application::~application() = default;

void application::run()
{
    // sig_set_ctrl sets the signal mask and signal event handler and enables systemd watchdog
    // its dtor will restore the old signal state
    sig_set_ctrl ssc{_sd_event_loop};

    pre_run();
    ssc.enable_watchdog();
    ::sd_event_loop(_sd_event_loop.get());
    post_run();
}

core::sd_event_loop const& application::get_sd_event() const
{
    return _sd_event_loop;
}






