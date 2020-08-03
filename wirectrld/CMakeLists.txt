
set(SRCS
    src/main.cpp
    src/opts.cpp
    src/config.cpp
)

add_executable(wirectrld "${SRCS}")

target_link_libraries(wirectrld
    PRIVATE core iniread
)
