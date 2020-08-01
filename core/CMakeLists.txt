

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

target_compile_options(core
    PUBLIC
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
            -Wall -Werror -Wextra -Wconversion -Wsign-conversion>
        $<$<CXX_COMPILER_ID:MSVC>: /W4 /WX>
)