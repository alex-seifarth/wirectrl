
set(SRCS
    src/reader.cpp
)

add_library(iniread STATIC "${SRCS}")

target_include_directories(iniread
    PUBLIC ./include
)

target_compile_options(iniread
    PUBLIC
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
            -Wall -Werror -Wextra -Wconversion -Wsign-conversion>
        $<$<CXX_COMPILER_ID:MSVC>: /W4 /WX>
)
