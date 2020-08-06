
set(SRCS
    core-tests.cpp
    tests-trim.cpp
    tests-read_line_ext.cpp
    test-ini_file.cpp
)

add_executable(test-libcore "${SRCS}")
target_link_libraries(test-libcore
    PRIVATE doctest core
)

add_test(NAME test-libcore COMMAND test-libcore)
