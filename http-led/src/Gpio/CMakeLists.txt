add_library(Gpio)

target_sources(Gpio
    PUBLIC
        FILE_SET HEADERS
        FILES
            Gpio.h
)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    # On the Pi: use the real pigpio library
    find_library(PIGPIO_LIB pigpio REQUIRED)
    target_link_libraries(Gpio PRIVATE ${PIGPIO_LIB})
else()
    # On macOS (or other non-Linux): use stubs so the project compiles
    target_sources(Gpio PRIVATE stubs/pigpio.cpp)
    target_include_directories(Gpio PRIVATE stubs)
endif()
