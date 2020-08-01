#include <iostream>

#include <core/dbus-application.h>
#include <string>

int main() {
    core::dbus_application app{core::DBusType::Session, "de.titnc.test"};
    app.run();


    std::cout << "Hello, World!" << std::endl;
    return EXIT_SUCCESS;
}
