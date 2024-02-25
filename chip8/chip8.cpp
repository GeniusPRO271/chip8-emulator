#include "chip8.hpp"
#include <vector>
#include <fstream>
#include <iostream>
const unsigned int FONTSET_SIZE = 80;
const unsigned int FONTSET_START_ADDRESS = 0x50;
const unsigned int START_ADDRESS = 0x200;

uint8_t fontset[FONTSET_SIZE] =
    {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

Chip8::Chip8()
    : randGen(std::chrono::system_clock::now().time_since_epoch().count())
{
    // Initialize PC
    Program_counter = START_ADDRESS;

    // Load fonts into memory
    for (unsigned int i = 0; i < FONTSET_SIZE; ++i)
    {
        ch8_memory[FONTSET_START_ADDRESS + i] = fontset[i];
    }

    // Initialize RNG
    randByte = std::uniform_int_distribution<uint8_t>(0, 255U);

    // Set up function pointer table
    table[0x0] = &Chip8::Table0;
    table[0x1] = &Chip8::Opcode1NNN;
    table[0x2] = &Chip8::Opcode2NNN;
    table[0x3] = &Chip8::Opcode3XNN;
    table[0x4] = &Chip8::Opcode4XNN;
    table[0x5] = &Chip8::Opcode5XY0;
    table[0x6] = &Chip8::Opcode6XNN;
    table[0x7] = &Chip8::Opcode7XNN;
    table[0x8] = &Chip8::Table8;
    table[0x9] = &Chip8::Opcode9XY0;
    table[0xA] = &Chip8::OpcodeANNN;
    table[0xB] = &Chip8::OpcodeBNNN;
    table[0xC] = &Chip8::OpcodeCXNN;
    table[0xD] = &Chip8::OpcodeDXYN;
    table[0xE] = &Chip8::TableE;
    table[0xF] = &Chip8::TableF;

    for (size_t i = 0; i <= 0xE; i++)
    {
        table0[i] = &Chip8::OP_NULL;
        table8[i] = &Chip8::OP_NULL;
        tableE[i] = &Chip8::OP_NULL;
    }

    table0[0x0] = &Chip8::Opcode00E0;
    table0[0xE] = &Chip8::Opcode00EE;

    table8[0x0] = &Chip8::Opcode8XY0;
    table8[0x1] = &Chip8::Opcode8XY1;
    table8[0x2] = &Chip8::Opcode8XY2;
    table8[0x3] = &Chip8::Opcode8XY3;
    table8[0x4] = &Chip8::Opcode8XY4;
    table8[0x5] = &Chip8::Opcode8XY5;
    table8[0x6] = &Chip8::Opcode8XY6;
    table8[0x7] = &Chip8::Opcode8XY7;
    table8[0xE] = &Chip8::Opcode8XYE;

    tableE[0x1] = &Chip8::OpcodeEXA1;
    tableE[0xE] = &Chip8::OpcodeEX9E;

    for (size_t i = 0; i <= 0x65; i++)
    {
        tableF[i] = &Chip8::OP_NULL;
    }

    tableF[0x07] = &Chip8::OpcodeFX07;
    tableF[0x0A] = &Chip8::OpcodeFX0A;
    tableF[0x15] = &Chip8::OpcodeFX15;
    tableF[0x18] = &Chip8::OpcodeFX18;
    tableF[0x1E] = &Chip8::OpcodeFX1E;
    tableF[0x29] = &Chip8::OpcodeFX29;
    tableF[0x33] = &Chip8::OpcodeFX33;
    tableF[0x55] = &Chip8::OpcodeFX55;
    tableF[0x65] = &Chip8::OpcodeFX65;
}

void Chip8::LoadROM(char const *filename)
{
    std::ifstream file(filename, std::ios::binary | std::ios::ate);

    if (file.is_open())
    {
        std::streampos size = file.tellg();
        char *buffer = new char[size];
        file.seekg(0, std::ios::beg);
        file.read(buffer, size);
        file.close();

        for (long i = 0; i < size; ++i)
        {
            ch8_memory[START_ADDRESS + i] = buffer[i];
        }

        delete[] buffer;
    }
}

void Chip8::Cycle()
{
    // Fetch
    opcode = (ch8_memory[Program_counter] << 8u) | ch8_memory[Program_counter + 1];

    // Increment the PC before we execute anything
    Program_counter += 2;

    // Decode and Execute
    ((*this).*(table[(opcode & 0xF000u) >> 12u]))();

    // Decrement the delay timer if it's been set
    if (Delay_timer > 0)
    {
        --Delay_timer;
    }

    // Decrement the sound timer if it's been set
    if (Sound_timer > 0)
    {
        --Sound_timer;
    }
}

void Chip8::Table0()
{
 
    ((*this).*(table0[opcode & 0x000Fu]))();
}

void Chip8::Table8()
{
 
    ((*this).*(table8[opcode & 0x000Fu]))();
}

void Chip8::TableE()
{

    ((*this).*(tableE[opcode & 0x000Fu]))();
}

void Chip8::TableF()
{

    ((*this).*(tableF[opcode & 0x00FFu]))();
}

void Chip8::OP_NULL()
{

}

// Clears the screen.
void Chip8::Opcode00E0()
{

    memset(video, 0, sizeof(V)); // set registers to 0
}

// Returns from a subroutine.
void Chip8::Opcode00EE()
{

    --Stack_Pointer;
    Program_counter = Stack[Stack_Pointer];
}

// Jumpst to address NNN
void Chip8::Opcode1NNN()
{

    Program_counter = opcode & 0x0FFF; // only interested in NNN of opcode 1NNN
}

void Chip8::Opcode2NNN()
{
    uint16_t address = opcode & 0x0FFF;

    Stack[Stack_Pointer] = Program_counter;
    ++Stack_Pointer;
    Program_counter = address;
}
void Chip8::Opcode3XNN()
{

    int regx = opcode & 0x0F00; // mask off reg X
    regx = regx >> 8;           // shift x across
    int nn = opcode & 0x00FF;   // mask off NN
    if (V[regx] == nn)
        Program_counter += 2; // skip next instruction
}

void Chip8::Opcode4XNN()
{

    int regx = opcode & 0x0F00; // mask off reg X
    regx = regx >> 8;           // shift x across
    int nn = opcode & 0x00FF;   // mask off NN
    if (V[regx] != nn)
        Program_counter += 2; // skip next instruction
}

void Chip8::Opcode5XY0()
{

    int regx = opcode & 0x0F00; // mask off reg x
    regx = regx >> 8;           // shift x across
    int regy = opcode & 0x00F0; // mask off reg y
    regy = regy >> 4;           // shift y across
    if (V[regx] == V[regy])
        Program_counter += 2; // skip next instruction
}

void Chip8::Opcode6XNN()
{

    int regx = opcode & 0x0F00; // mask off reg X
    regx = regx >> 8;           // shift x across
    int nn = opcode & 0x00FF;   // mask off NN
    V[regx] = nn;               // Sets VX to NN
}

void Chip8::Opcode7XNN()
{

    int regx = opcode & 0x0F00; // mask off reg X
    regx = regx >> 8;           // shift x across
    int nn = opcode & 0x00FF;   // mask off NN
    V[regx] += nn;              // Adds NN to VX
}

void Chip8::Opcode8XY0()
{

    int regx = opcode & 0x0F00; // mask off reg X
    regx = regx >> 8;           // shift x across
    int regy = opcode & 0x00F0; // mask off Y
    regy = regy >> 4;
    V[regx] = V[regy]; // Adds NN to VX
}

void Chip8::Opcode8XY1()
{

    int regx = opcode & 0x0F00; // mask off reg X
    regx = regx >> 8;           // shift x across
    int regy = opcode & 0x00F0; // mask off Y
    regy = regy >> 4;
    V[regx] |= V[regy]; // bitwise OR operation
}

void Chip8::Opcode8XY2()
{
    
    int regx = opcode & 0x0F00; // mask off reg X
    regx = regx >> 8;           // shift x across
    int regy = opcode & 0x00F0; // mask off Y
    regy = regy >> 4;
    V[regx] &= V[regy]; // bitwise AND operation
}

void Chip8::Opcode8XY3()
{
   
    int regx = opcode & 0x0F00; // mask off reg X
    regx = regx >> 8;           // shift x across
    int regy = opcode & 0x00F0; // mask off Y
    regy = regy >> 4;
    V[regx] ^= V[regy]; // Sets VX to VX xor VY
}

void Chip8::Opcode8XY4()
{
    u8 Vx = (opcode & 0x0F00) >> 8;
    u8 Vy = (opcode & 0x00F0) >> 4;

    u16 sum = V[Vx] + V[Vy];

    if (sum > 255)
    {
        V[0xF] = 1;
    }
    else
    {
        V[0xF] = 0;
    }

    V[Vx] = sum & 0xFF;
}

void Chip8::Opcode8XY5()
{
    u8 Vx = (opcode & 0x0F00) >> 8;
    u8 Vy = (opcode & 0x00F0) >> 4;

    if (V[Vx] > V[Vy])
    {
        V[0xF] = 1;
    }
    else
    {
        V[0xF] = 0;
    }

    V[Vx] -= V[Vy];
}

void Chip8::Opcode8XY6()
{
    
    int regx = opcode & 0x0F00; // mask off reg X
    regx = regx >> 8;           // shift x across

    V[0xF] = V[regx] & 0x1;

    V[regx] >>= 1;
}

void Chip8::Opcode8XY7()
{
    u8 Vx = (opcode & 0x0F00) >> 8;
    u8 Vy = (opcode & 0x00F0) >> 4;

    if (V[Vy] > V[Vx])
    {
        V[0xF] = 1;
    }
    else
    {
        V[0xF] = 0;
    }

    V[Vx] = V[Vy] - V[Vx];
}

void Chip8::Opcode8XYE()
{
    
    int regx = opcode & 0x0F00; // mask off reg X
    regx = regx >> 8;           // shift x across

    V[0xF] = (V[regx] & 0x80) >> 7; // // Save MSB in VF

    V[regx] <<= 1;
}

void Chip8::Opcode9XY0()
{

    int regx = opcode & 0x0F00; // mask off reg X
    regx = regx >> 8;           // shift x across
    int regy = opcode & 0x00F0; // mask off Y
    regy = regy >> 4;

    int xval = V[regx];
    int yval = V[regy];

    if (xval != yval)
    {
        Program_counter += 2; // skip next instruction
    }
}

void Chip8::OpcodeANNN()
{

    int NNN = opcode & 0x0FFF; // mask off reg NNN

    Address_I = NNN;
}

void Chip8::OpcodeBNNN()
{

    int NNN = opcode & 0x0FFF; // mask off reg NNN

    Program_counter = V[0] + NNN;
}

void Chip8::OpcodeCXNN()
{

    int regx = opcode & 0x0F00; // mask off reg NNN
    regx = regx >> 8;
    int NN = opcode & 0x00FF; // mask off reg NNN

    // Perform bitwise AND with NN and store the result in VX
    V[regx] = randByte(randGen) & NN;
}

void Chip8::OpcodeDXYN()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    uint8_t height = opcode & 0x000Fu;

    // Wrap if going beyond screen boundaries
    uint8_t xPos = V[Vx] % VIDEO_WIDTH;
    uint8_t yPos = V[Vy] % VIDEO_HEIGHT;

    V[0xF] = 0;

    for (unsigned int row = 0; row < height; ++row)
    {
        uint8_t spriteByte = ch8_memory[Address_I + row];

        for (unsigned int col = 0; col < 8; ++col)
        {
            uint8_t spritePixel = spriteByte & (0x80u >> col);
            uint32_t *screenPixel = &video[(yPos + row) * VIDEO_WIDTH + (xPos + col)];

            // Sprite pixel is on
            if (spritePixel)
            {
                // Screen pixel also on - collision
                if (*screenPixel == 0xFFFFFFFF)
                {
                    V[0xF] = 1;
                }

                // Effectively XOR with the sprite pixel
                *screenPixel ^= 0xFFFFFFFF;
            }
        }
    }
}

void Chip8::OpcodeEX9E()
{

    int regx = opcode & 0x0F00;
    regx = regx >> 8;

    int key = V[regx];

    if (keypad[key])
    {
        Program_counter += 2;
    }
}

void Chip8::OpcodeEXA1()
{
  
    int regx = opcode & 0x0F00;
    regx = regx >> 8;

    int key = V[regx];

    if (!keypad[key])
    {
        Program_counter += 2;
    }
}

void Chip8::OpcodeFX07()
{

    int regx = opcode & 0x0F00; // mask off reg NNN
    regx = regx >> 8;

    V[regx] = Delay_timer;
}

void Chip8::OpcodeFX0A()
{
 
    int regx = opcode & 0x0F00; // mask off reg NNN
    regx = regx >> 8;

    if (keypad[0])
    {
        V[regx] = 0;
    }
    else if (keypad[1])
    {
        V[regx] = 1;
    }
    else if (keypad[2])
    {
        V[regx] = 2;
    }
    else if (keypad[3])
    {
        V[regx] = 3;
    }
    else if (keypad[4])
    {
        V[regx] = 4;
    }
    else if (keypad[5])
    {
        V[regx] = 5;
    }
    else if (keypad[6])
    {
        V[regx] = 6;
    }
    else if (keypad[7])
    {
        V[regx] = 7;
    }
    else if (keypad[8])
    {
        V[regx] = 8;
    }
    else if (keypad[9])
    {
        V[regx] = 9;
    }
    else if (keypad[10])
    {
        V[regx] = 10;
    }
    else if (keypad[11])
    {
        V[regx] = 11;
    }
    else if (keypad[12])
    {
        V[regx] = 12;
    }
    else if (keypad[13])
    {
        V[regx] = 13;
    }
    else if (keypad[14])
    {
        V[regx] = 14;
    }
    else if (keypad[15])
    {
        V[regx] = 15;
    }
    else
    {
        Program_counter -= 2;
    }
}

void Chip8::OpcodeFX15()
{

    int regx = opcode & 0x0F00; // mask off reg NNN
    regx = regx >> 8;

    Delay_timer = V[regx];
}

void Chip8::OpcodeFX18()
{


    int regx = opcode & 0x0F00; // mask off reg NNN
    regx = regx >> 8;

    Sound_timer = V[regx];
}

void Chip8::OpcodeFX1E()
{

    int regx = opcode & 0x0F00; // mask off reg NNN
    regx = regx >> 8;

    Address_I += V[regx];
}

void Chip8::OpcodeFX29()
{
    
    int regx = opcode & 0x0F00; // mask off reg NNN
    regx = regx >> 8;

    Address_I = FONTSET_START_ADDRESS + (5 * V[regx]);
}

void Chip8::OpcodeFX33()
{

    int regx = opcode & 0x0F00;
    regx >>= 8;

    int value = V[regx];

    int hundreds = value / 100;
    int tens = (value / 10) % 10;
    int units = value % 10;

    ch8_memory[Address_I] = hundreds;
    ch8_memory[Address_I + 1] = tens;
    ch8_memory[Address_I + 2] = units;
}

void Chip8::OpcodeFX55()
{
    int regx = opcode & 0x0F00;
    regx >>= 8;
    for (int i = 0; i <= regx; i++)
    {
        ch8_memory[Address_I + i] = V[i];
    }
}

void Chip8::OpcodeFX65()
{
    
    int regx = opcode & 0x0F00; // mask off reg NNN
    regx = regx >> 8;

    for (u8 i = 0; i <= regx; ++i)
    {
        V[i] = ch8_memory[Address_I + i];
    }
}
