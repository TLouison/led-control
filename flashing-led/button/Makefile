all: build run

build:
	g++ -Wall -pthread -o button.o button.cpp -lpigpio -lrt

run:
	sudo ./button.o
