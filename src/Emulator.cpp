#include "Common/GBA.hpp"
#include "CPU/CPU.hpp"

#include <iostream>
#include <cstdio>

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cout << "You must specify a ROM file to load." << std::endl;
        return 0;
    }

    FILE* rom = fopen(argv[1], "rb");

    if (!rom)
    {
        std::cout << "The file you have specified is invalid." << std::endl;
        return 0;
    }

    GBAHeader header;
    int read = fread(&header, sizeof(GBAHeader), 1, rom);
    
    if (!read)
    {
        std::cout << "Could not read from file." << std::endl;
        return 0;
    }

    if (!header.VerifyChecksum())
    {
        std::cout << "The specified file is not a valid GBA ROM." << std::endl;
        return 0;
    }

    CPU arm7(CPUMode::Interpreter);
    arm7.LoadROM(header, rom);

    fclose(rom);

    arm7.Run();

	return 0;
}