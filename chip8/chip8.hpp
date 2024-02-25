#ifndef CHIP8_HPP
#define CHIP8_HPP
#include <chrono>
#include <random>

typedef unsigned char u8;
typedef unsigned short int u16;
const unsigned int KEY_COUNT = 16;
const unsigned int MEMORY_SIZE = 4096;
const unsigned int REGISTER_COUNT = 16;
const unsigned int STACK_LEVELS = 16;
const unsigned int VIDEO_HEIGHT = 32;
const unsigned int VIDEO_WIDTH = 64;

class Chip8
{
public:
    Chip8();
    void LoadROM(char const *filename);
    void Cycle();

    uint8_t keypad[KEY_COUNT]{};
    uint32_t video[VIDEO_WIDTH * VIDEO_HEIGHT]{};

private:
    void Table0();
    void Table8();
    void TableE();
    void TableF();
    // Do nothing
    void OP_NULL();

    // Clears the screen.
    void Opcode00E0();

    // Returns from a subroutine.
    void Opcode00EE();

    // Jumpst to address NNN
    void Opcode1NNN();

    // Calls subroutine NNN
    void Opcode2NNN();

    // Skips the next instruction if VX equals NN (usually the next instruction is a jump to skip a code block).
    void Opcode3XNN();

    // Skips the next instruction if VX does not equal NN (usually the next instruction is a jump to skip a code block).
    void Opcode4XNN();

    // Skips the next instruction if VX equals VY (usually the next instruction is a jump to skip a code block).
    void Opcode5XY0();

    // Sets VX to NN.
    void Opcode6XNN();

    // Adds NN to VX (carry flag is not changed).
    void Opcode7XNN();

    // Sets VX to the value of VY.
    void Opcode8XY0();

    // Sets VX to VX or VY
    void Opcode8XY1();

    // Sets VX to VX and VY.
    void Opcode8XY2();

    // Sets VX to VX xor VY.
    void Opcode8XY3();

    // Adds VY to VX. VF is set to 1 when there's an overflow, and to 0 when there is not.
    void Opcode8XY4();

    // VY is subtracted from VX. VF is set to 0 when there's an underflow, and 1 when there is not. (i.e. VF set to 1 if VX >= VY and 0 if not).
    void Opcode8XY5();

    // Stores the least significant bit of VX in VF and then shifts VX to the right by 1
    void Opcode8XY6();

    // Sets VX to VY minus VX. VF is set to 0 when there's an underflow, and 1 when there is not. (i.e. VF set to 1 if VY >= VX).
    void Opcode8XY7();

    // Stores the most significant bit of VX in VF and then shifts VX to the left by 1
    void Opcode8XYE();

    // Skips the next instruction if VX does not equal VY.
    void Opcode9XY0();

    // Sets I to the address NNN.
    void OpcodeANNN();

    // Jumps to the address NNN plus V0.
    void OpcodeBNNN();

    // Sets VX to the result of a bitwise and operation on a random number (Typically: 0 to 255) and NN.
    void OpcodeCXNN();

    // Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels. Each row of 8 pixels is read as bit-coded starting from memory location I; I value does not change after the execution of this instruction. As described above, VF is set to 1 if any screen pixels are flipped from set to unset when the sprite is drawn, and to 0 if that does not happen
    void OpcodeDXYN();

    // Skips the next instruction if the key stored in VX is pressed (usually the next instruction is a jump to skip a code block).
    void OpcodeEX9E();

    // Skips the next instruction if the key stored in VX is not pressed (usually the next instruction is a jump to skip a code block).
    void OpcodeEXA1();

    // Sets VX to the value of the delay timer.
    void OpcodeFX07();

    // A key press is awaited, and then stored in VX (blocking operation, all instruction halted until next key event).
    void OpcodeFX0A();

    // Sets the delay timer to VX.
    void OpcodeFX15();

    // Sets the sound timer to VX.
    void OpcodeFX18();

    // Adds VX to I. VF is not affected
    void OpcodeFX1E();

    // Sets I to the location of the sprite for the character in VX. Characters 0-F (in hexadecimal) are represented by a 4x5 font
    void OpcodeFX29();

    // Stores the binary-coded decimal representation of VX, with the hundreds digit in memory at location in I, the tens digit at location I+1, and the ones digit at location I+2.
    void OpcodeFX33();

    // Stores from V0 to VX (including VX) in memory, starting at address I. The offset from I is increased by 1 for each value written, but I itself is left unmodified.
    void OpcodeFX55();

    // Fills from V0 to VX (including VX) with values from memory, starting at address I. The offset from I is increased by 1 for each value read, but I itself is left unmodified.
    void OpcodeFX65();

    // Memory 4096 (0x1000) memory locations, all of which are 8 bits
    u8 ch8_memory[0x1000]{};

    u8 V[16]{}; // Registers
    u16 Address_I{};
    u16 Program_counter{};
    u8 Delay_timer{};
    u8 Sound_timer{};
    u16 Stack[STACK_LEVELS]{};
    u8 Stack_Pointer{};
    u16 opcode{};

    std::default_random_engine randGen;
    std::uniform_int_distribution<uint8_t> randByte;

    typedef void (Chip8::*Chip8Func)();
    Chip8Func table[0xF + 1];
    Chip8Func table0[0xE + 1];
    Chip8Func table8[0xE + 1];
    Chip8Func tableE[0xE + 1];
    Chip8Func tableF[0x65 + 1];
};

#endif