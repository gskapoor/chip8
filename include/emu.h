#ifndef EMU_H
#define EMU_H

#define CHIP8_NUM_DATA_REGISTERS 0x10
#define PC_START 0x200
#define CHIP8_MEMORY_SIZE 0x1000
#define CHIP8_RECURSION_DEPTH 256

#include "common.h"
#include "display.h"

class Emulator {
public:
    Emulator();
    ~Emulator();

    Display display;

    // void run(const std::vector<uint16_t>&);

private:
    std::array<uint8_t, CHIP8_MEMORY_SIZE> memory;
    std::array<uint8_t, CHIP8_NUM_DATA_REGISTERS> registers;

    uint16_t iReg; // Should bitmask by 0xFFF every time you do a memory access
    uint16_t programCounter = PC_START; // Should bitmask by 0xFFF every time you do a memory access
    
    std::array<uint16_t, CHIP8_RECURSION_DEPTH> callStack;
    uint8_t stackPointer = 0;

    uint8_t delayTimer = 0;
    uint8_t soundTimer = 0;

    std::array<std::array<uint8_t, 32 / 8>, 64 / 8> displayBuffer; // I'm writing it like this because we only need one bit to hold the display

};

#endif