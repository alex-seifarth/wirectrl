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

#include <core/sd_event_loop.h>

namespace core {

    //! \brief Application base class that initialises sd-event loop and signals.
    //! Users should subclass the application class to provide their own initialisation
    //! and termination code.
    //!
    class application {
    public:
        application();
        ~application();

        application(application const &) = delete;
        application &operator=(application const &) = delete;

        //! Enters the sd-event loop until a terminating signal is received or the app terminates the loop.
        //! This method first calls pre_run before it enters the sd_event loop and
        //! calls post_run after the loop returns.
        virtual void run();

    protected:
        //! Called before the sd-event loop is run.
        virtual void pre_run() = 0;

        //! Called after the sd-event loop returned.
        virtual void post_run() = 0;

        //! Returns the used sd_event loop for the current application object.
        //! \note The application uses always the thread's default sd_event.
        sd_event_loop const& get_sd_event() const;

    private:
        sd_event_loop _sd_event_loop;
    };

} // namespace core
