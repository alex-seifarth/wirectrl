find_package(PkgConfig REQUIRED)
pkg_check_modules(libgpiod REQUIRED IMPORTED_TARGET GLOBAL libgpiod)


set(SRCS
    src/main.cpp
    src/opts.cpp
    src/config.cpp
    src/application.cpp
    src/gpio.cpp
)

add_executable(wirectrld "${SRCS}")

target_link_libraries(wirectrld
    PRIVATE core gpiod
)
