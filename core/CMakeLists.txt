

set(SRCS
    src/exception.cpp
    src/sd_event_loop.cpp
    src/application.cpp
    src/dbus-application.cpp
    src/final.cpp
)

add_library(core STATIC "${SRCS}")

target_include_directories(core
    PUBLIC ./include
)

target_link_libraries(core
    PUBLIC systemd
)
