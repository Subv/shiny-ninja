#include "Common/GBA.hpp"
#include "Common/Files.hpp"
#include "Common/Utilities.hpp"
#include "Memory.hpp"
#include "CPU/CPU.hpp"
#include "GPU/GPU.hpp"

#include <cstring>
#include <memory>
#include <cstdint>

MMU::MMU(CPU* arm) : _cpu(arm)
{
}

void MMU::LoadROM(GBAHeader& header, FILE* rom, FILE* bios)
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

    // Cleanup memory
    memset(_ioram, 0, sizeof(_ioram) / sizeof(uint8_t));
    memset(_bios, 0, sizeof(_bios) / sizeof(uint8_t));
    memset(_ewram, 0, sizeof(_ewram) / sizeof(uint8_t));
    memset(_iwram, 0, sizeof(_iwram) / sizeof(uint8_t));
    memset(_vram, 0, sizeof(_vram) / sizeof(uint8_t));
    memset(_sram, 0, sizeof(_sram) / sizeof(uint8_t));

    // Load BIOS
    fread(&_bios, sizeof(uint8_t), sizeof(_bios) / sizeof(uint8_t), bios);
}

uint32_t MMU::ReadUInt32(uint32_t offset)
{
    return ReadUInt16(offset) | (ReadUInt16(offset + 2) << 16);
}

uint16_t MMU::ReadUInt16(uint32_t address)
{
    return ReadUInt8(address) | (ReadUInt8(address + 1) << 8);
}

uint8_t MMU::ReadUInt8(uint32_t address)
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
            return _ewram[address - 0x02000000];
        case 0x3: // On-Chip WRAM
            Utilities::Assert(address <= 0x03007FFF, "Trying to read in unused IWRAM memory");
            return _iwram[address - 0x03000000];
        case 0x4: // I/O Registers
            Utilities::Assert(address <= 0x040003FF, "Trying to read in unused IOMAP memory");
            return _ioram[(address & 0xFFF) % 0x400];
        case 0x5: // BG/OBJ Palette RAM
            Utilities::Assert(address <= 0x050003FF, "Trying to read in unused palette memory");
            return _cpu->GetGPU()->ReadInt8(address);
        case 0x6: // VRAM
            Utilities::Assert(address <= 0x06017FFF, "Trying to read in unused VRAM memory");
            return _cpu->GetGPU()->ReadInt8(address);
        case 0x7: // OAM - OBJ Attributes
            Utilities::Assert(address <= 0x070003FF, "Trying to read in unused OBJ memory");
            return _cpu->GetGPU()->ReadInt8(address);
        case 0x8: // Game Pak, State 0
        case 0x9: // Game Pak, State 0
        case 0xA: // Game Pak, State 1
        case 0xB: // Game Pak, State 1
        case 0xC: // Game Pak, State 2
        case 0xD: // Game Pak, State 2
            // ((0x8, 0x9) - 0x8) >> 1 = 0
            // ((0xA, 0xB) - 0x8) >> 1 = 1
            // ((0xC, 0xD) - 0x8) >> 1 = 2
            return _pakROM[(address - 0x08000000) >> 25][address % 0x02000000];
        case 0xE: // Game Pak SRAM
            Utilities::Assert(address <= 0x0E00FFFF, "Trying to read in unused SRAM memory");
            return _sram[address - 0x0E000000];
    }
    return 0;
}

void MMU::WriteUInt32(uint32_t address, uint32_t value)
{
    WriteUInt16(address, (value >> 16) & 0xFFFF);
    WriteUInt16(address + 2, value & 0xFFFF);
}

void MMU::WriteUInt16(uint32_t address, uint16_t value)
{
    WriteUInt8(address, (value >> 8) & 0xFF);
    WriteUInt8(address + 1, value & 0xFF);
}

void MMU::WriteUInt8(uint32_t address, uint8_t value)
{
    switch ((address & 0x0F000000) >> 24)
    {
        case 0x0: // Bios - System ROM
            // (00004000-01FFFFFF)
            Utilities::Assert(false, "Trying to write in BIOS.");
        case 0x2: // On-Board WRAM
            // (02040000-02FFFFFF)
            Utilities::Assert(address <= 0x0203FFFF, "Trying to write in unused EWRAM memory");
            // Accessing unused memory returns the recently pre-fetched opcode, ie. the
            // 32bit opcode at $+8 in ARM state, or the 16bit-opcode at $+4 in THUMB state,
            // in the later case the 16bit opcode is mirrored across both upper/lower 16bits
            // of the returned 32bit data.
            _ewram[address - 0x02000000] = value;
            break;
        case 0x3: // On-Chip WRAM
            Utilities::Assert(address <= 0x03007FFF, "Trying to write in unused IWRAM memory");
            _iwram[address - 0x03000000] = value;
            break;
        case 0x4: // I/O Registers
            Utilities::Assert(address <= 0x040003FF, "Trying to write in unused IOMAP memory");
            
            // The way the GBA handles the Interrupt Request Flags makes this code necessary, writing 1 to the bits in this address will both enable and disable interrupt requests
            if (address >= InterruptRequestFlags && address < InterruptRequestFlags + 2)
                _ioram[(address & 0xFFF) % 0x400] ^= value;
            else
                _ioram[(address & 0xFFF) % 0x400] = value;

            break;
        case 0x5: // BG/OBJ Palette RAM
            Utilities::Assert(address <= 0x050003FF, "Trying to write in unused palette memory");
            _cpu->GetGPU()->WriteUInt8(address, value);
            break;
        case 0x6: // VRAM
            Utilities::Assert(address <= 0x06017FFF, "Trying to write in unused VRAM memory");
            _cpu->GetGPU()->WriteUInt8(address, value);
            break;
        case 0x7: // OAM - OBJ Attributes
            Utilities::Assert(address <= 0x070003FF, "Trying to write in unused OBJ memory");
            _cpu->GetGPU()->WriteUInt8(address, value);
            break;
        case 0x8: // Game Pak, State 0
        case 0x9: // Game Pak, State 0
        case 0xA: // Game Pak, State 1
        case 0xB: // Game Pak, State 1
        case 0xC: // Game Pak, State 2
        case 0xD: // Game Pak, State 2
            // ((0x8, 0x9) - 0x8) >> 1 = 0
            // ((0xA, 0xB) - 0x8) >> 1 = 1
            // ((0xC, 0xD) - 0x8) >> 1 = 2
            _pakROM[(address - 0x08000000) >> 25][address % 0x02000000] = value;
            break;
        case 0xE: // Game Pak SRAM
            Utilities::Assert(address <= 0x0E00FFFF, "Trying to write in unused SRAM memory");
            _sram[address - 0x0E000000] = value;
            break;
    }
}

