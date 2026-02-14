cmake_minimum_required(VERSION 3.31)

project(GpioRest)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS Off)

add_compile_options(-g -Wall -Wextra -Wpedantic)

find_package(httplib CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

add_executable(GpioRest)
target_sources(GpioRest
  PRIVATE
    src/main.cpp
    src/api.cpp
)

target_link_libraries(GpioRest PRIVATE httplib::httplib)
target_link_libraries(GpioRest PRIVATE nlohmann_json::nlohmann_json)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    # On the Pi: use the real pigpio library
    find_library(PIGPIO_LIB pigpio REQUIRED)
    target_link_libraries(GpioRest PRIVATE ${PIGPIO_LIB})
else()
    # On macOS (or other non-Linux): use stubs so the project compiles
    target_sources(GpioRest PRIVATE src/stubs/pigpio.cpp)
    target_include_directories(GpioRest PRIVATE src/stubs)
endif()
