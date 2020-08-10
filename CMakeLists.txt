cmake_minimum_required(VERSION 3.16)
project(wirectrl VERSION 0.1)

set(CMAKE_CXX_STANDARD 17)
include(cmake/os-detect.cmake)

option(OPT_GTEST "Build gtests" ON)
if (OPT_GTEST)
    enable_testing()
    set(BUILD_TESTING ON)
endif()

add_subdirectory(external)
add_subdirectory(core)
add_subdirectory(wirectrld)
