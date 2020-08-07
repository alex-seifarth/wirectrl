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
#include "config.h"
#include "application.h"

#include <core/ini.h>
#include <core/exception.h>

#include <systemd/sd-journal.h>

#include <cstdlib>
#include <stdexcept>


int main(int argc, char* argv[])
{
    configuration config;
    try {
        opts options = parse_program_options(argc, argv);
        core::ini::file ini_file;
        std::string path;
        std::tie(ini_file, path) = get_config(options);
        sd_journal_print(LOG_INFO, "Reading configuration from %s", path.c_str());
        config = configuration::decode_from_section(ini_file);

        sd_journal_print(LOG_INFO, "DBus configuration '%s', '%s', '%s'",
                         config.dbus.connection_name.c_str(), config.dbus.object_name.c_str(),
                         config.dbus.use_session_bus ? "session" : "system");
        int gpio_cnt{0};
        for (auto const& gpio : config.gpios) {
            sd_journal_print(LOG_INFO, "gpio #%i: %s-%i (%s)",
                             gpio_cnt, gpio.gpio_chip_name.c_str(), gpio.gpio_line_id, gpio.name.c_str());
            ++gpio_cnt;
        }
    }
    catch(core::ini::parse_exception& e) {
        sd_journal_print(LOG_ERR, "Failed to parse ini configuration file. (%s) [line %i, expression: %s]"
                         , e.what(), e.line_number(), e.expression().c_str());
        return EXIT_FAILURE;
    }
    catch (std::runtime_error& e) {
        sd_journal_print(LOG_ERR, "Wirectrl initialisation failure. (%s)", e.what());
        return EXIT_FAILURE;
    }

    try {
        application app{config};
        app.run();
    }
    catch (core::runtime_exception& e) {
        sd_journal_print(LOG_ERR, "application failure: %s", e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
