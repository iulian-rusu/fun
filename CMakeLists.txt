cmake_minimum_required(VERSION 3.16)
project(fun LANGUAGES VERSION 2.0 CXX)

set(PROJECT_DESCRIPTION "Small C++ utility library.")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_COMPILER cc)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compiler options
if (MSVC)
    add_compile_options(/W4)
    if (CMAKE_BUILD_TYPE MATCHES Debug)
        add_compile_options(/Od)
    else()
        add_compile_options(/O2)
    endif()
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Wuninitialized -Wshadow -Wunused)
    if (CMAKE_BUILD_TYPE MATCHES Debug)
        add_compile_options(-Og)
    else()
        add_compile_options(-O3)
    endif()
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-fconcepts-diagnostics-depth=3)
endif()

# Main library target
add_library(fun INTERFACE)
target_include_directories(fun INTERFACE include/)

# Building tests
option(FUN_BUILD_TESTS "build tests" ON)
if(${FUN_BUILD_TESTS})
    add_subdirectory(test/)
endif()

# Building examples
option(FUN_BUILD_EXAMPLES "build examples" ON)
if (${FUN_BUILD_EXAMPLES})
    add_subdirectory(example/)
endif()

# Installation
install(DIRECTORY include/fun DESTINATION include FILES_MATCHING PATTERN *.hpp)