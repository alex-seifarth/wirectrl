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

#include <csignal>
#include <systemd/sd-event.h>

#include <core/sd_event_loop.h>
#include <core/exception.h>

#include <array>
#include <vector>

namespace core {

    //! Blocks the
    struct sig_set_ctrl
    {
        explicit sig_set_ctrl(core::sd_event_loop& event_loop_)
            : _event_loop{event_loop_}
            , _old_set{}
        {
            int r;
            std::array<int, 2> signals{SIGTERM, SIGINT};

            sigemptyset(&_old_set);
            sigset_t new_set;
            sigemptyset(&new_set);
            for (auto const& signal : signals) {
                r = sigaddset(&new_set, signal);
                if (r < 0)
                    throw core::runtime_exception{"cannot add signal", r};
            }
            r = sigprocmask(SIG_BLOCK, &new_set, &_old_set);
            if (r < 0)
                throw core::runtime_exception{"cannot set signal set", r};

            for (auto const& signal : signals) {
                _signal_event_sources.push_back(nullptr);
                r = sd_event_add_signal(event_loop_.get(), &_signal_event_sources.back(), signal, nullptr, nullptr);
                if (r < 0)
                    throw core::runtime_exception{"Cannot set signal handler", r};
            }
        }

        void enable_watchdog() const
        {
            int r = sd_event_set_watchdog(_event_loop.get(), true);
            if (r < 0)
                throw core::runtime_exception{"unable to setup watchdog", r};
        }

        ~sig_set_ctrl()
        {
            sd_event_set_watchdog(_event_loop.get(), false);

            for(auto& evs : _signal_event_sources) {
                sd_event_source_unref(evs);
            }
            _signal_event_sources.clear();
            sigprocmask(SIG_SETMASK, &_old_set, nullptr);
        }

        core::sd_event_loop _event_loop;
        sigset_t _old_set;
        std::vector<sd_event_source*> _signal_event_sources{};
    };

} // namespace core
