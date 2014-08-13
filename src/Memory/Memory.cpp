#include "Memory.hpp"

#include "Common/GBA.hpp"
#include "Common/Files.hpp"

void MMU::LoadROM(GBAHeader& header, FILE* rom)
{
    // Zero out all 3 wait states
    for (int i = 0; i < NUM_GAMEPAK_WAITSTATES; ++i)
        memset(&_pakROM[i], 0, sizeof(_pakROM[i]));

    // Copy the ROM to the GamePak memory
    uint32_t size = Files::GetFileSize(rom) - sizeof(header);

    // Copy the header to the wait state
    memcpy(&_pakROM[0], &header, sizeof(GBAHeader));

    // Read the data from the file into the first wait state
    fread(&_pakROM[0][sizeof(GBAHeader)], sizeof(uint8_t), size, rom);

    // Now mirror it into the other wait states
    for (int i = 1; i < NUM_GAMEPAK_WAITSTATES; ++i)
        memcpy(&_pakROM[i], _pakROM[0], size + sizeof(GBAHeader));
}

uint32_t MMU::Read32(uint32_t address)
{
    // Determine if we're trying to read from one of the ROM wait states
    
    if (address >= 0x0C000000) // We're trying to read from Wait State 2
        return *(uint32_t*)(&_pakROM[2][address - 0x0C000000]); // Read 4 bytes from there
    else if (address >= 0x0A000000) // We're reading from Wait State 1
        return *(uint32_t*)(&_pakROM[1][address - 0x0A000000]); // Read 4 bytes from there
    else if (address >= 0x08000000) // We're trying to read from Wait State 0
        return *(uint32_t*)(&_pakROM[1][address - 0x08000000]); // Read 4 bytes from there

    return 0;
}

uint16_t MMU::Read16(uint32_t address)
{
    // Determine if we're trying to read from one of the ROM wait states

    if (address >= 0x0C000000) // We're trying to read from Wait State 2
        return *(uint16_t*)(&_pakROM[2][address - 0x0C000000]); // Read 2 bytes from there
    else if (address >= 0x0A000000) // We're reading from Wait State 1
        return *(uint16_t*)(&_pakROM[1][address - 0x0A000000]); // Read 2 bytes from there
    else if (address >= 0x08000000) // We're trying to read from Wait State 0
        return *(uint16_t*)(&_pakROM[1][address - 0x08000000]); // Read 2 bytes from there

    return 0;
}

