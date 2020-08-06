
#include "opts.h"
#include "config.h"

#include <core/dbus-application.h>
#include <core/ini.h>

#include <systemd/sd-journal.h>

#include <cstdlib>
#include <iostream>
#include <stdexcept>


int main(int argc, char* argv[])
{
    try {
        opts options = parse_program_options(argc, argv);
        core::ini::file ini_file;
        std::string path;
        std::tie(ini_file, path) = get_config(options);
        sd_journal_print(LOG_INFO, "Reading configuration from %s", path.c_str());
        auto configuration = configuration::decode_from_section(ini_file);
        (void)configuration;
        std::cout << "connection name: " << configuration.dbus.connection_name << "\n";
        std::cout << "object     name: " << configuration.dbus.object_name << "\n";
        std::cout << "use_session_bus: " << std::boolalpha << configuration.dbus.use_session_bus << "\n";

    }
    catch (std::runtime_error& e) {
        sd_journal_print(LOG_ERR, "Wirectrl initialisation failure. (%s)", e.what());
        std::cerr << e.what() << "\n";
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
