#include <iostream>
#include <cstdlib>
#include <ctime>
#include <SDL.h>
#include "chip8/chip8.hpp"
#include <random>
#include "chip8/platform.hpp"

// Window dimensions
static const int WINDOW_WIDTH = 64;
static const int WINDOW_HEIGHT = 32;

int main(int argc, char **argv)
{
    if (argc != 4)
    {
        std::cerr << "Usage: " << argv[0] << " <Scale> <Delay> <ROM>\n";
        std::exit(EXIT_FAILURE);
    }

    int videoScale = std::stoi(argv[1]);
    int cycleDelay = std::stoi(argv[2]);
    char const *romFilename = argv[3];


    Platform platform("CHIP-8 Emulator", WINDOW_WIDTH*videoScale, WINDOW_HEIGHT*videoScale, WINDOW_WIDTH, WINDOW_HEIGHT);



    Chip8 chip8;

    chip8.LoadROM(romFilename);

    int videoPitch = sizeof(chip8.video[0]) * WINDOW_WIDTH;

    auto lastCycleTime = std::chrono::high_resolution_clock::now();
    bool quit = false;

    while (!quit)
    {
        quit = platform.ProcessInput(chip8.keypad);


        auto currentTime = std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();
 

        if (dt > cycleDelay)
        {
            lastCycleTime = currentTime;
            chip8.Cycle();
            platform.Update(chip8.video, videoPitch);
        }
    }

    return 0;
}
