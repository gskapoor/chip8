#include "emu.h"

#include <algorithm>

// Starts the emulator up
Emulator::Emulator() {

    // Make sure to set all entries in the display to 0
    for (auto &row: displayBuffer){
        for (uint8_t &elem: row){
            elem = 0;
        }
    }

    std::array<u_int8_t, 80> font = { 
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

    std::copy(font.begin(), font.end(), memory.begin());

}

Emulator::~Emulator() {
}

void Emulator::loadROM(const std::vector<uint16_t> &instructions) {
    // First we gotta check if the space is too small
    if (instructions.size() * 2 > 0x1000 - 0x200 ){
        throw std::runtime_error("Error loading ROM: Too many instructions");
    }


    for (size_t i = 0; i < instructions.size(); i++){
        uint8_t instLhs = static_cast<uint8_t>(instructions[i] >> 8);
        uint8_t instRhs = static_cast<uint8_t>(instructions[i] & 0xFF);

        memory[0x200 + 2 * i] = instLhs;
        memory[0x200 + 2 * i + 1] = instRhs;
    }
}

void Emulator::draw(uint8_t x, uint8_t y, uint8_t n){

    uint8_t flippedOneBit = 0;
    // ASSUMPTION: no rollover here
    if (x >= 64 || y >= 32){
        // Too big, just return
        registers[0xF] = 0;
        return;
    }

    if (x % 8 == 0){
        for (uint8_t j = 0; j + y < 32 && j < n; j++ ){
            if ((displayBuffer[j+y][x / 8] & memory[(iReg & 0xFFF) + j ]) != 0){
                flippedOneBit = 1;
            }
            displayBuffer[j+y][x/8] ^= memory[(iReg & 0xFFF) + j];
        }
    } else {
        throw std::logic_error("not implemented");
    }

    registers[0xF] = flippedOneBit;


}

void Emulator::step(){
    // First, get the instruction
    // This is at memory[PC:PC+2]
    uint16_t instruction = (static_cast<uint16_t>(memory[programCounter % CHIP8_MEMORY_SIZE]) << 8)
    | static_cast<uint16_t>(memory[(programCounter + 1u) % CHIP8_MEMORY_SIZE]);


    programCounter += 2;

    switch (instruction & 0xF000) {
        uint8_t reg, val, xReg, yReg, xVal, yVal, numRows;
        case 0x0000:
            if (instruction == 0x00E0){
                // clear the screen
                for (auto &row: displayBuffer){
                    for (uint8_t &elem: row){
                        elem = 0;
                    }
                }
    
            }
            break;
        case 0x1000:
            // This is always a jump instruction
            programCounter = static_cast<uint8_t>(instruction & 0xFFF);
            break;
        case 0x6000:
            // This is a "set immediate" instruction
            reg = static_cast<uint8_t>(instruction &0xF00) >> 8;
            val = static_cast<uint8_t>(instruction & 0xFF);
            registers[reg] = val;
            break;
        case 0x7000:
            // This is an "add immediate" instruction
            reg = static_cast<uint8_t>(instruction &0xF00) >> 8;
            val = static_cast<uint8_t>(instruction & 0xFF);
            registers[reg] += val;
            break;
        case 0xA000:
            // This is a "set immediate" instruction
            // for the "I" register
            iReg = instruction & 0xFFF;
            break;
        case 0xD000:
            // This is complex enough to merit it's own function
            // This is the "sprite draw" function
            xReg = static_cast<uint8_t>(instruction &0xF00) >> 8;
            yReg = static_cast<uint8_t>(instruction &0xF0) >> 4;

            xVal = registers[xReg];
            yVal = registers[yReg];
            numRows = instruction & 0xF;

            draw(xVal, yVal, numRows);

            break;
        default:
            break;
    };

    display.update(displayBuffer);


}

void Emulator::run(){
    state = ACTIVE;

    while(state != HALT){
        step();
    }
}