cmake_minimum_required(VERSION 3.16)
project(wirectrl VERSION 0.1)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(core)
add_subdirectory(iniread)
add_subdirectory(wirectrld)

add_executable(wirectrl main.cpp)
target_link_libraries(wirectrl
    PRIVATE systemd core iniread)