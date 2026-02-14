all: build run

build:
	g++ -Wall -pthread -o raii.o main.cpp gpio_pin.cpp -lpigpio -lrt

run:
	sudo ./raii.o
