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
    
    // Read the data from the file into the first wait state
    fread(&_pakROM[0], sizeof(uint8_t), size, rom);

    // Now mirror it into the other wait states
    for (int i = 1; i < NUM_GAMEPAK_WAITSTATES; ++i)
        memcpy(&_pakROM[i], _pakROM[0], size);
}

