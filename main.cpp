#include <iostream>

#include <core/dbus-application.h>
#include <string>

int main() {
    core::dbus_application app{};
    app.run();


    std::cout << "Hello, World!" << std::endl;
    return EXIT_SUCCESS;
}
