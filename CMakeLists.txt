cmake_minimum_required(VERSION 3.16)
project(wirectrl VERSION 0.1)

include(cmake/systemd.cmake)

add_subdirectory(core)
add_subdirectory(iniread)

set(CMAKE_CXX_STANDARD 17)

add_executable(wirectrl main.cpp)
target_link_libraries(wirectrl
    PRIVATE systemd core iniread)