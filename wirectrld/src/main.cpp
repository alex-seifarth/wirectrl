
#include "opts.h"
#include "config.h"

#include <core/dbus-application.h>

#include <systemd/sd-journal.h>

#include <cstdlib>
#include <iostream>
#include <stdexcept>


int main(int argc, char* argv[])
{
    try {
        opts options = parse_program_options(argc, argv);
        iniread::section root_section;
        std::string path;
        std::tie(root_section, path) = get_config(options);
        sd_journal_print(LOG_INFO, "Reading configuration from %s", path.c_str());
    }
    catch (std::runtime_error& e) {
        std::cerr << e.what() << "\n";
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
