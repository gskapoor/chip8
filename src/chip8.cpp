#include <fstream>

#include "common.h"
#include "emu.h"

int main(int argc, char** argv){

	// Want exactly one argument (for now): the file
	// Maybe change this to a visual/drag and drop system

	if (argc < 2) {
		std::cerr << "Usage: chip8 <program>" << std::endl;
		return 1;
	}

	std::ifstream romFile(argv[1], 
		std::ios::binary | std::ios::ate
	);

	uint32_t size = (uint32_t)romFile.tellg();
	romFile.seekg(0);

	std::vector<uint16_t> rom(size/2 + (size % 2), 0);

	romFile.read(reinterpret_cast<char*>(rom.data()), size);

	Emulator myEmulator(std::unique_ptr<Display>(std::make_unique<SDLDisplay>()));

	myEmulator.loadROM(rom);
	myEmulator.run();

	return 0;

}