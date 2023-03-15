cmake_minimum_required(VERSION 3.24)
project(error-reporting C)
enable_testing()

set(CMAKE_C_STANDARD 99)
set(CMAKE_BUILD_TYPE Debug)
set(C_FLAGS "-Wall -Wuninitialized -Wmissing-include-dirs -Wextra -Wconversion -Werror -Wfatal-errors -Wformat")

# Path configuration

set(BIN_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(LIB_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

# Add library to the project

add_library(error_reporting STATIC
        src/error_reporting.c
        src/error_reporting.h)

# Add executable to the project

add_executable(test_lib src/test_error_reporting.c)
target_link_libraries(test_lib error_reporting)
set_target_properties(
        test_lib
        PROPERTIES
        COMPILE_FLAGS -Wall
        ARCHIVE_OUTPUT_DIRECTORY ${LIB_DIRECTORY}
        EXCLUDE_FROM_ALL off
        DEPENDS error_reporting)

# Set properties for all executables

set_target_properties(
        test_lib
        PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BIN_DIRECTORY}
                   EXCLUDE_FROM_ALL off)

# Create the list of programs to execute in order to run the tests suite

add_test(test_lib  ${BIN_DIRECTORY}/test_lib)

