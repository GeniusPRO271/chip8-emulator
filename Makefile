CXX = g++
CXXFLAGS = -std=c++2a -Wall -pedantic $(shell sdl2-config --cflags)
LDLIBS = $(shell sdl2-config --libs)
EXECUTABLE = your_executable_name
SRC_DIR = src
CHIP8_DIR = chip8

# List of source files
SOURCES = main.cpp $(CHIP8_DIR)/chip8.cpp $(CHIP8_DIR)/platform.cpp

# List of header files
HEADERS = $(CHIP8_DIR)/chip8.hpp $(CHIP8_DIR)/platform.hpp

# Default ARGV
VIDEO_SCALE = 20
CYCLE_DELAY = 1
ROM_FILE = roms/WormV4.ch8


all: build run

build: $(EXECUTABLE)

$(EXECUTABLE): $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $@ $(LDLIBS)

run:
	./$(EXECUTABLE) $(VIDEO_SCALE) $(CYCLE_DELAY) $(ROM_FILE)

clean:
	rm -f $(EXECUTABLE)
