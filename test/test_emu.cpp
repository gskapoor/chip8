#include "doctest.h"

#include "common.h"
#include "emu.h"

TEST_CASE("Basic Register/ALU Operations [(6-8/A)XXX]"){

    Emulator emulator(std::unique_ptr<Display>(std::make_unique<MockDisplay>()));

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
    SUBCASE("Load I"){
        std::vector<uint16_t> rom = {
            0x1EA0 // register[0] = register[0] << 1
        };
        emulator.loadROM(rom);
        emulator.step();
        CHECK(emulator.getI() == 0x01E);
    }
}

TEST_CASE("Jump Instructions"){

    Emulator emulator(std::unique_ptr<Display>(std::make_unique<MockDisplay>()));

    SUBCASE("Call and Return"){
        emulator.setRegister(0, 0);
        emulator.setRegister(1, 1);
        std::vector<uint16_t> rom = {0x0422, 0x0160, 0x0061, 0xEE00};
        emulator.loadROM(rom);
        emulator.step();
        CHECK(emulator.getPC() == 0x204);
        emulator.step();
        CHECK(emulator.getRegister(0) == 0);
        CHECK(emulator.getRegister(1) == 0);

        // Now test Return
        emulator.step(2);
        CHECK(emulator.getRegister(0) == 1);
    }
    SUBCASE("Jump") {
        emulator.setRegister(0, 0);
        emulator.setRegister(1, 1);
        std::vector<uint16_t> rom = {0x0412, 0x0160, 0x0061};
        emulator.loadROM(rom);
        emulator.step();
        CHECK(emulator.getPC() == 0x204);
        emulator.step();
        CHECK(emulator.getRegister(0) == 0);
        CHECK(emulator.getRegister(1) == 0);
    }
    SUBCASE("Skip if immediate"){
        std::vector<uint16_t> rom = {0x0031, 0x0060, 0x0160};
        emulator.loadROM(rom);
        SUBCASE("reg1 == imm"){
            emulator.setRegister(1, 0);
            emulator.step();
            CHECK(emulator.getPC() == 0x204);
            emulator.step();
            CHECK(emulator.getRegister(0) == 1);
        }
        SUBCASE("reg1 != imm"){
            emulator.setRegister(1, 1);
            emulator.step();
            CHECK(emulator.getPC() == 0x202);
            emulator.step();
            CHECK(emulator.getRegister(0) == 0);
        }
    }
    SUBCASE("Skip if not immediate"){
        std::vector<uint16_t> rom = {0x0041, 0x0060, 0x0160};
        emulator.loadROM(rom);
        SUBCASE("reg1 != imm"){
            emulator.setRegister(1, 1);
            emulator.step();
            CHECK(emulator.getPC() == 0x204);
            emulator.step();
            CHECK(emulator.getRegister(0) == 1);
        }
        SUBCASE("reg1 == imm"){
            emulator.setRegister(1, 0);
            emulator.step();
            CHECK(emulator.getPC() == 0x202);
            emulator.step();
            CHECK(emulator.getRegister(0) == 0);
        }
    }
    SUBCASE("Skip if registers"){
        std::vector<uint16_t> rom = {0x0051, 0x0062, 0x0162};
        emulator.loadROM(rom);
        emulator.setRegister(0, 0);
        SUBCASE("reg0 == reg1"){
            emulator.setRegister(1, 0);
            emulator.step();
            CHECK(emulator.getPC() == 0x204);
            emulator.step();
            CHECK(emulator.getRegister(2) == 1);
        }
        SUBCASE("reg0 != reg1"){
            emulator.setRegister(1, 1);
            emulator.step();
            CHECK(emulator.getPC() == 0x202);
            emulator.step();
            CHECK(emulator.getRegister(2) == 0);
        }
    }
    SUBCASE("Jump by offset imediate"){
        std::vector<uint16_t> rom = {0x02B0};
        emulator.loadROM(rom);
        emulator.setRegister(0, 2);

        emulator.step();
        CHECK(emulator.getPC() == 4);
    }
}

TEST_CASE("Drawing"){

    Emulator emulator(std::unique_ptr<Display>(std::make_unique<MockDisplay>()));
    std::array<std::array<uint8_t, 64 / 8>, 32> buffer;
    SUBCASE("Basic Drawing with Collision"){
        buffer[0][0] = 0b00110011;
        std::vector<uint16_t> rom = {0x01D0, 0b01010101};
        emulator.loadROM(rom);

        emulator.setI(0x202); // draws the above
        emulator.setDisplayBuffer(buffer);
        emulator.step();

        CHECK(emulator.getDisplayBuffer()[0][0] == 0b01100110);

        CHECK(emulator.getRegister(0xF) == 1);
    }
    SUBCASE("Basic Drawing with no Collision"){
        buffer[0][0] = 0b00110011;
        std::vector<uint16_t> rom = {0x01D0, 0b11001100};
        emulator.loadROM(rom);

        emulator.setI(0x202); // draws the above
        emulator.setDisplayBuffer(buffer);
        emulator.step();

        CHECK(emulator.getDisplayBuffer()[0][0] == 0xFF);

        CHECK(emulator.getRegister(0xF) == 0);
    }
    SUBCASE("Staggered Drawing with no Collision"){
        buffer[0][0] = 0b0011;
        buffer[0][1] = 0b0011'0000;
        std::vector<uint16_t> rom = {0x11D0, 0b01010101};
        emulator.loadROM(rom);

        emulator.setI(0x202); // draws the above
        emulator.setRegister(0, 4);
        emulator.setDisplayBuffer(buffer);
        emulator.step();

        CHECK(emulator.getDisplayBuffer()[0][0] == 0b0110);
        CHECK(emulator.getDisplayBuffer()[0][1] == 0b0110'0000);

        CHECK(emulator.getRegister(0xF) == 1);
    }
    SUBCASE("Staggered Drawing with no Collision"){
        buffer[0][0] = 0x0;
        buffer[0][1] = 0x0;
        std::vector<uint16_t> rom = {0x11D0, 0xFF};
        emulator.loadROM(rom);

        emulator.setI(0x202); // draws the above
        emulator.setRegister(0, 4);
        emulator.setDisplayBuffer(buffer);
        emulator.step();

        CHECK(emulator.getDisplayBuffer()[0][0] == 0xF);
        CHECK(emulator.getDisplayBuffer()[0][1] == 0xF0);

        CHECK(emulator.getRegister(0xF) == 0);
    }

}