#ifndef EMU_H
#define EMU_H

#define CHIP8_NUM_DATA_REGISTERS 0x10
#define PC_START 0x200
#define CHIP8_MEMORY_SIZE 0x1000
#define CHIP8_RECURSION_DEPTH 256

#include "common.h"
#include "display.h"

enum EmulatorState {
    ACTIVE,
    PAUSE,
    HALT
};

class Emulator {
public:
    Emulator();
    ~Emulator();

    Display display;
    EmulatorState state = PAUSE;

    // TODO: Consider making this a uint8_t array instead, it would be cleaner
    void loadROM(const std::vector<uint16_t>&);

    void step();
    void step(uint8_t);

    void run();

    void setRegister(uint8_t, uint8_t);
    uint8_t getRegister(uint8_t);

    void setPC(uint16_t);
    uint16_t getPC();

private:
    std::array<uint8_t, CHIP8_MEMORY_SIZE> memory{};
    std::array<uint8_t, CHIP8_NUM_DATA_REGISTERS> registers{};

    uint16_t iReg = 0; // Should bitmask by 0xFFF every time you do a memory access
    uint16_t programCounter = PC_START; // Should bitmask by 0xFFF every time you do a memory access
    
    std::array<uint16_t, CHIP8_RECURSION_DEPTH> callStack{};
    uint16_t stackPointer = 0;

    uint8_t delayTimer = 0;
    uint8_t soundTimer = 0;

    // I'm writing it like this because we only need one bit to hold the display
    std::array<std::array<uint8_t, 64 / 8 >, 32> displayBuffer{}; 

    void draw(uint8_t, uint8_t, uint8_t);

    void aluOp(uint8_t, uint8_t, uint8_t);

};

#endif