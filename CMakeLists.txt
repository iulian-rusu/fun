cmake_minimum_required(VERSION 3.17)
project(functional)

set(CMAKE_CXX_STANDARD 20)

if (MSVC)
    add_compile_options(/W4)
    add_compile_options(/O2)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Wuninitialized -Wshadow -Wunused)
    add_compile_options(-O3)
endif()

include_directories(include)
add_executable(functional main.cpp)