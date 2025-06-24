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
   
    // std::cout << instructions.size() << std::endl;    
    // std::cout << std::endl;

    if (instructions.size() * 2 > 0x1000 - 0x200 ){
        throw std::runtime_error("Error loading ROM: Too many instructions");
    }


    for (size_t i = 0; i < instructions.size(); i++){
        uint8_t instRhs = static_cast<uint8_t>(instructions[i] >> 8);
        uint8_t instLhs = static_cast<uint8_t>(instructions[i] & 0xFF);

        memory[0x200 + 2 * i] = instLhs;
        memory[0x200 + 2 * i + 1] = instRhs;
    }
}

void Emulator::draw(uint8_t x, uint8_t y, uint8_t n){

    uint8_t flippedOneBit = 0;
    // ASSUMPTION: no rollover here
    // TODO: We actualy should have rollover... 
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

            // std::cout << "printing at: " << x/8 << ", " << y / 1 << std::endl;
        }
    } else {
        uint8_t lOffset = x % 8;
        uint8_t rOffset = 8 - (x % 8);
        uint8_t lIndex = x / 8;
        // TODO: FIX BELOW
        uint8_t rIndex = x / 8 != 7 ? x / 8 + 1 : 0; // "<8" to wrap around if x + 1 == 8

        for (uint8_t j = 0; j + y < 32 && j < n; j++ ){
            if (((displayBuffer[j+y][lIndex] & memory[(iReg & 0xFFF) + j ]) >> lOffset
               | (displayBuffer[j+y][rIndex] & memory[(iReg & 0xFFF) + j ]) << rOffset
            ) != 0){

                // NOTE: DO NOT TRUST THIS CODE: IT IS BAD AND WILL HURT YOU
                flippedOneBit = 1;
            }
            displayBuffer[j+y][lIndex] ^= static_cast<uint8_t>(memory[(iReg & 0xFFF) + j] >> lOffset);
            displayBuffer[j+y][rIndex] ^= static_cast<uint8_t>(memory[(iReg & 0xFFF) + j] << rOffset);

            // std::cout << "printing at: " << x/8 << ", " << y / 1 << std::endl;
        }
    }

    registers[0xF] = flippedOneBit;
}

void Emulator::aluOp(uint8_t x, uint8_t y, uint8_t op){
    // All the two register operations are here, 
    // this function may be badly named

    switch (op) {
        case 0x0:
            registers[x] = registers[y];
            break;
        case 0x1:
            registers[x] |= registers[y];
            break;
        case 0x2:
            registers[x] &= registers[y];
            break;
        case 0x3:
            registers[x] ^= registers[y];
            break;
        case 0x4:
            registers[x] += registers[y];
            // A nifty overflow check
            registers[0xF] = registers[y] > registers[x] ? 1 : 0;
            break;
        case 0x5:
            registers[0xF] = registers[y] > registers[x] ? 1 : 0;
            registers[x] -= registers[y];
            break;
        case 0x6:
            registers[0xF] = registers[x] % 2;
            registers[x] >>= 1;
            break;
        case 0x7:
            registers[0xF] = registers[x] > registers[y] ? 1 : 0;
            registers[x] = registers[y] - registers[x];
            // same overflow trick, but with -Ry instead
            break;
        case 0xE:
            registers[0xF] = (registers[x] & 0x80) >> 7;
            registers[x] <<= 1;
            break;
        default:
            break;
    }
}

void Emulator::step(){
    // First, get the instruction
    // This is at memory[PC:PC+2]
    uint16_t instruction = (static_cast<uint16_t>(memory[programCounter % CHIP8_MEMORY_SIZE]) << 8)
    | static_cast<uint16_t>(memory[(programCounter + 1u) % CHIP8_MEMORY_SIZE]);

    // std::cout << "PC LOC: " << std::hex << std::uppercase << programCounter << std::endl;

    programCounter += 2;

    // std::cout << "CURRENT INS: " << std::hex << std::uppercase << instruction << std::endl;

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
    
            } else if (instruction == 0x00EE){
                // return insruction
                // First decrement the SP:

                stackPointer -= 1;
                // TODO: Check if this rollover is the correct behavior (it likely isn't)
                stackPointer %= CHIP8_RECURSION_DEPTH;

                programCounter = callStack[stackPointer];

            }
            break;
        case 0x1000:
            // This is always a jump instruction
            // TODO: Uncomment the below line later
            // programCounter = instruction & 0xFFF;
            break;
        case 0x2000:
            // Call instruction

            assert( 1 < CHIP8_RECURSION_DEPTH);

            callStack[stackPointer] = programCounter;
            stackPointer += 1;

            programCounter = instruction & 0xFFF;

            break;
        case 0x3000:
            // This is a "jump if equal" instruction
            reg = static_cast<uint8_t>((instruction &0xF00) >> 8);
            val = static_cast<uint8_t>(instruction & 0xFF);
            if (registers[reg] == val) {
                programCounter += 2;
            }
            
            break;
        case 0x6000:
            // This is a "set immediate" instruction
            reg = static_cast<uint8_t>((instruction &0xF00) >> 8);
            val = static_cast<uint8_t>(instruction & 0xFF);
            registers[reg] = val;
            break;
        case 0x7000:
            // This is an "add immediate" instruction
            reg = static_cast<uint8_t>((instruction &0xF00) >> 8);
            val = static_cast<uint8_t>(instruction & 0xFF);
            registers[reg] += val;
            break;

        case 0x8000:
            xReg  = static_cast<uint8_t>((instruction &0xF00) >> 8);
            yReg  = static_cast<uint8_t>((instruction &0xF0) >> 4);
            val = static_cast<uint8_t>(instruction &0xF);

            aluOp(xReg, yReg, val);
            break;
        case 0x9000:
            xReg  = static_cast<uint8_t>((instruction &0xF00) >> 8);
            yReg  = static_cast<uint8_t>((instruction &0xF0) >> 4);

            if (registers[xReg] != registers[yReg]){
                programCounter += 2;
            }
            
            break;
        case 0xA000:
            // This is a "set immediate" instruction
            // for the "I" register
            iReg = instruction & 0xFFF;
            break;
        case 0xB000:
            // jump to nnn + V0
            programCounter = (instruction & 0xFFF) + registers[0];
            break;
        case 0xC000:
            // TODO: figure out how to do random stuff:
            break;
        case 0xD000:
            // This is complex enough to merit it's own function
            // This is the "sprite draw" function
            xReg = static_cast<uint8_t>((instruction &0xF00) >> 8);
            yReg = static_cast<uint8_t>((instruction &0xF0) >> 4);

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

void Emulator::step(uint8_t numSteps){
    for (uint8_t i = 0; i < numSteps; i++){
        step();
    }
}

void Emulator::run(){
    state = ACTIVE;

    while(state != HALT){
        step();
        if (programCounter >= CHIP8_MEMORY_SIZE){
            state = HALT;
        }
    }
}

void Emulator::setRegister(uint8_t reg, uint8_t value){
    assert(reg < CHIP8_NUM_DATA_REGISTERS);

    registers[reg] = value;
}

uint8_t Emulator::getRegister(uint8_t reg){
    return registers[reg];
}
