#include <fstream>

#include "common.h"
#include "emu.h"

int main(int argc, char** argv){

	// Want exactly one argument (for now): the file
	// Maybe change this to a visual/drag and drop system

	if (argc < 2) {
		std::cout << "Usage: chip8 <program>" << std::endl;
	}

	std::ifstream romFile(argv[0], std::ios::binary);
	std::vector<uint16_t> rom;

	
	

	

    Emulator myEmulator;
	myEmulator.run();

	return 0;

}