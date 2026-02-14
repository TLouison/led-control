target_sources(GpioRest
  PRIVATE
    src/main.cpp
)

target_link_libraries(GpioRest
    PRIVATE
        Gpio
        Server
)

add_subdirectory(Gpio)
add_subdirectory(Server)
