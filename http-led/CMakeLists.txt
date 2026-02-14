cmake_minimum_required(VERSION 3.31)

project(GpioRest)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS Off)

add_compile_options(-g -Wall -Wextra -Wpedantic)

add_subdirectory(src)
