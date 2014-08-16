#include "Common/GBA.hpp"
#include "Common/Files.hpp"
#include "Common/Utilities.hpp"
#include "Memory.hpp"
#include "CPU/CPU.hpp"
#include "GPU/GPU.hpp"

#include <cstring>
#include <memory>
#include <cstdint>

MMU::MMU(std::shared_ptr<CPU> arm) : _cpu(arm), _inBios(false)
{
}

void MMU::LoadROM(GBAHeader& header, FILE* rom)
{
    // Zero out all 3 wait states
    for (int i = 0; i < NUM_WAIT_STATES; ++i)
        memset(&_pakROM[i], 0, sizeof(_pakROM[i]));

    // Copy the ROM to the GamePak memory
    uint32_t size = Files::GetFileSize(rom) - sizeof(header);

    // Copy the header to the wait state
    memcpy(&_pakROM[0], &header, sizeof(GBAHeader));

    // Read the data from the file into the first wait state
    fread(&_pakROM[0][sizeof(GBAHeader)], sizeof(uint8_t), size, rom);

    // Now mirror it into the other wait states
    for (int i = 1; i < NUM_WAIT_STATES; ++i)
        memcpy(&_pakROM[i], _pakROM[0], size + sizeof(GBAHeader));
}

uint32_t MMU::ReadInt32(uint32_t offset)
{
    return ReadInt16(offset) | (ReadInt16(offset + 4) << 16);
}

uint16_t MMU::ReadInt16(uint32_t address)
{
    switch ((address & 0x0F000000) >> 24)
    {
        case 0x0: // Bios - System ROM
            // (00004000-01FFFFFF)
            Utilities::Assert(address <= 0x3FFF, "Trying to read in unused BIOS memory");
            if (_cpu->GetRegister(PC) < 0x400) // Reading here is allowed.
                return _bios[address];
            // Reading from the BIOS is allowed IFF the Program Counter is located inside
            // the BIOS. If not, reading will return the most recent successfully fetched
            // BIOS opcode (eg. the opcode at [00DCh+8] after startup and SoftReset, the
            // opcode at [0134h+8] during IRQ execution, and opcode at [013Ch+8] after IRQ
            // execution, and opcode at [0188h+8] after SWI execution).
            return 0;
        case 0x2: // On-Board WRAM
            // (02040000-02FFFFFF)
            Utilities::Assert(address <= 0x0203FFFF, "Trying to read in unused EWRAM memory");
            // Accessing unused memory returns the recently pre-fetched opcode, ie. the
            // 32bit opcode at $+8 in ARM state, or the 16bit-opcode at $+4 in THUMB state,
            // in the later case the 16bit opcode is mirrored across both upper/lower 16bits
            // of the returned 32bit data.
            return *(uint16_t*)(&_ewram[address - 0x02000000]);
        case 0x3: // On-Chip WRAM
            // (03008000-03FFFFFF)
            Utilities::Assert(address <= 0x03007FFF, "Trying to read in unused IWRAM memory");
            return *(uint16_t*)(&_iwram[address - 0x03000000]);
        case 0x4: // I/O Registers
            // (04000400-04FFFFFF)
            Utilities::Assert(address <= 0x040003FF, "Trying to read in unused IOMAP memory");
            // See case 0x2 comment.
            return 0; // NYI
        case 0x5: // BG/OBJ Palette RAM
            // (05000400-05FFFFFF)
            Utilities::Assert(address <= 0x050003FF, "Trying to read in unused palette memory");
            return sGPU->ReadInt16(address);
        case 0x6: // VRAM
            // (06018000-06FFFFFF)
            Utilities::Assert(address <= 0x06017FFF, "Trying to read in unused VRAM memory");
            return sGPU->ReadInt16(address);
        case 0x7: // OAM - OBJ Attributes
            // (07000400-07FFFFFF)
            Utilities::Assert(address <= 0x070003FF, "Trying to read in unused OBJ memory");
            return sGPU->ReadInt16(address);
        case 0x8: // Game Pak, State 0
        case 0x9: // Game Pak, State 0
        case 0xA: // Game Pak, State 1
        case 0xB: // Game Pak, State 1
        case 0xC: // Game Pak, State 2
        case 0xD: // Game Pak, State 2
            // ((0x8, 0x9) - 0x8) >> 1 = 0
            // ((0xA, 0xB) - 0x8) >> 1 = 1
            // ((0xC, 0xD) - 0x8) >> 1 = 2
            return *(uint16_t*)(&_pakROM[(address - 0x08000000) >> 25][address & 0x0FFFFFFF]);
        case 0xE: // Game Pak SRAM
            if (address > 0x0E00FFFF)
                return 0;
            return *(uint16_t*)(&_sram[address - 0x0E000000]);
    }
    return 0;
}

