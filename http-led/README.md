# HTTP LED

This project was designed to integrate my already existing web development skills
with the hardware development skills I am working on. This spins up a simple webserver
using `cpp-httplib` to serve requests that can:

1. Get the "status" of the LED (the GPIO level)
2. Turn the light on
3. Turn the light off

## Building

The build of this program is Dockerized, allowing you to build the executable
easily. To do so, simply run:

```bash
docker buildx build --output type=local,dest=./output .
```

This will build the image and then output the binary from it to `/output/GpioRest`.
I directly copy this binary to my Pi, and then run it with `sudo ./GpioRest` since
it needs `sudo` permission to access the GPIO pins.

### Note

This was the first of the hardware projects I've worked on where I wrote
it on MacOS during development (the earlier projects were written in Vim on the
Pi itself) and then pulled and built it on the target device (the Raspberry Pi 3B+).

I've built out a full `cmake` project for this. Using `vcpkg`, you can download
the required libraries for the HTTP server easily during setup. This doesn't
currently support cross-compilation as the program is only being run on the Pi,
but I intend to set future projects up to cross-compile to speed up development!

## API Docs

- `GET /led/status` returns current LED level (0 for off, 1 for on)
- `POST /led/on` turns the LED on
- `POST /led/off` turns the LED off
- `POST /stop` kills the server and cleanly exits the program

## Profiling

After finishing the program, I used `valgrind` to step execute the program. After testing all API commands and exiting cleanly, the `valgrind` showed no leaked memory.

## Debugging

No difficult debugging was needed for this project, but I did use `gdb` to get a better sense of the way smart pointers (specifically `unique_ptr`) are constructed and destructed.

Of note:

- Construction:
    - `std::make_unique` returns a `std::unique_ptr` with the provided class
    - Each parameter of the class is sent to `std::forward` to `static_cast` those values
- Destruction
    - The destructor code works like normal
    - After the destructor exits, the destructor for the `unique_ptr` executes,

## Learnings

- Stubbing hardware dependencies for local compilation/testing
- Basic web servers in C++
- CMake basics
    - CMakeLists.txt
    - CMakePresets.json
    - `cmake --profile default && cmake --build build`
    - Using `vcpkg` to install third-party libaries `cpp-httplib` and `nlohmann-json`)
- C++ lambda functions
