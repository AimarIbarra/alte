cmake_minimum_required(VERSION 3.26)

# Name subject to change
project(alte C)

# We will be using the gnu17 standard,
# so feel free tonuse gnu extensions.
# We may migrate to gnu2x in the future.
set(CMAKE_C_STANDARD 17)
add_compile_options(-Wall -Wextra -Werror)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY
    ${CMAKE_SOURCE_DIR})

add_subdirectory(src)
