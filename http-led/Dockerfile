FROM debian AS build

WORKDIR /app

# Install CMake
RUN apt-get update && apt-get install -y build-essential git cmake wget curl zip unzip pkg-config python3-setuptools

# Install vcpkg
RUN git clone https://github.com/microsoft/vcpkg.git "/vcpkg" && cd /vcpkg && ./bootstrap-vcpkg.sh
ENV VCPKG_ROOT="/vcpkg"
ENV PATH="$PATH:$VCPKG_ROOT"

# Install pigpio
RUN wget https://github.com/joan2937/pigpio/archive/master.zip
RUN unzip master.zip
RUN cd pigpio-master && make && make install

# Copy CMake config files
COPY . .

# Install C++ packages
RUN cmake --preset default
RUN cmake -B build

# Build CMake build directory
RUN cmake --build build

FROM scratch

COPY --from=build /app/build/src/GpioRest .
