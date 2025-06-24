#include "doctest.h"

#include "common.h"
#include "emu.h"

TEST_CASE("Basic Register/ALU Operations [(6-9)XXX]"){

    Emulator emulator;
    SUBCASE("Load Immediate") {
        std::vector<uint16_t> rom = {
            0xFF60 
        };
        emulator.loadROM(rom);
        emulator.step();
        CHECK(emulator.getRegister(0) == 0xFF);
    }
    SUBCASE("Add Immediate") {
        emulator.setRegister(0, 0x1);
        std::vector<uint16_t> rom = {
            0x0170 
        };
        emulator.loadROM(rom);
        emulator.step();
        CHECK(emulator.getRegister(0) == 0x2);
    }
    SUBCASE("Load Registers") {
        emulator.setRegister(0, 0x0);
        emulator.setRegister(1, 0x1);
        std::vector<uint16_t> rom = {
            0x1080  // register[0] = register[1] = 1
        };
        emulator.loadROM(rom);
        emulator.step();
        CHECK(emulator.getRegister(0) == 0x1);
    }
    SUBCASE("Or Registers") {
        emulator.setRegister(0, 0b0011);
        emulator.setRegister(1, 0b0101);
        std::vector<uint16_t> rom = {
            0x1180  // register[0] = register[1] = 1
        };
        emulator.loadROM(rom);
        emulator.step();
        CHECK(emulator.getRegister(0) == 0b0111);
    }
    SUBCASE("And Registers") {
        emulator.setRegister(0, 0b0011);
        emulator.setRegister(1, 0b0101);
        std::vector<uint16_t> rom = {
            0x1280  // register[0] = register[1] = 1
        };
        emulator.loadROM(rom);
        emulator.step();
        CHECK(emulator.getRegister(0) == 0b0001);
    }
    SUBCASE("Xor Registers") {
        emulator.setRegister(0, 0b0011);
        emulator.setRegister(1, 0b0101);
        std::vector<uint16_t> rom = {
            0x1380  // register[0] = register[1] = 1
        };
        emulator.loadROM(rom);
        emulator.step();
        CHECK(emulator.getRegister(0) == 0b0110);
    }
    SUBCASE("Add Registers") {
        emulator.setRegister(0, 1);
        emulator.setRegister(1, 1);
        std::vector<uint16_t> rom = {
            0x1480  // register[0] = register[1] = 1
        };
        emulator.loadROM(rom);
        emulator.step();
        CHECK(emulator.getRegister(0) == 2);
    }
    SUBCASE("Subtract Registers") {
        // We also need to test overflow here
        emulator.setRegister(0, 0);
        emulator.setRegister(1, 1);
        emulator.setRegister(0xF, 0);

        SUBCASE("Overflow"){
            std::vector<uint16_t> rom = {
                0x1580  // register[0] = register[1] = 1
            };
            emulator.loadROM(rom);
            emulator.step();
            CHECK(emulator.getRegister(0) == 0xFF);
            CHECK(emulator.getRegister(0xF) == 1);
        }
        SUBCASE("Noverflow"){
            std::vector<uint16_t> rom = {
                0x0581  // register[0] = register[1] = 1
            };
            emulator.loadROM(rom);
            emulator.step();
            CHECK(emulator.getRegister(1) == 1);
            CHECK(emulator.getRegister(0xF) == 0);
        }
    }
    SUBCASE("Shift Right") {
        // Two Subcases: 1 in LSB or 0
        std::vector<uint16_t> rom = {
            0x1680 // register[0] = register[0] >> 1
        };
        emulator.loadROM(rom);
        SUBCASE("0 in LSB"){
            emulator.setRegister(0, 0b10);
            emulator.step();
            CHECK(emulator.getRegister(0x0) == 1);
            CHECK(emulator.getRegister(0xF) == 0);
        }
        SUBCASE("1 in LSB"){
            emulator.setRegister(0, 0b11);
            emulator.step();
            CHECK(emulator.getRegister(0x0) == 1);
            CHECK(emulator.getRegister(0xF) == 1);
        }
    }
    SUBCASE("Subtract and Negate Registers") {
        // We also need to test overflow here
        emulator.setRegister(0, 0);
        emulator.setRegister(1, 1);
        emulator.setRegister(0xF, 0);

        SUBCASE("Overflow"){
            std::vector<uint16_t> rom = {
                0x0781
            };
            emulator.loadROM(rom);
            emulator.step();
            CHECK(emulator.getRegister(1) == 0xFF);
            CHECK(emulator.getRegister(0xF) == 1);
        }
        SUBCASE("Noverflow"){
            std::vector<uint16_t> rom = {
                0x1780 
            };
            emulator.loadROM(rom);
            emulator.step();
            CHECK(emulator.getRegister(0) == 1);
            CHECK(emulator.getRegister(0xF) == 0);
        }
    }
    SUBCASE("Shift Left") {
        // Two Subcases: 1 in LSB or 0
        std::vector<uint16_t> rom = {
            0x1E80 // register[0] = register[0] << 1
        };
        emulator.loadROM(rom);
        SUBCASE("0 in MSB"){
            emulator.setRegister(0, 0b10);
            emulator.step();
            CHECK(emulator.getRegister(0x0) == 0b100);
            CHECK(emulator.getRegister(0xF) == 0);
        }
        SUBCASE("1 in MSB"){
            emulator.setRegister(0, 0xFF);
            emulator.step();
            CHECK(emulator.getRegister(0x0) == 0xFE);
            CHECK(emulator.getRegister(0xF) == 1);
        }
    }
}