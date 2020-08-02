#include <iostream>

#include <core/dbus-application.h>
#include <iniread/iniread.h>

#include <string>
#include <fstream>
#include <iostream>

int main() {
    core::dbus_application app{core::DBusType::Session, "de.titnc.test"};
    //app.run();

    std::ifstream ini_file{"test.conf"};
    iniread::read_ini_file(ini_file);

    std::cout << "Hello, World!" << std::endl;
    return EXIT_SUCCESS;
}
