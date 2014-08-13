#ifndef MEMORY_HPP
#define MEMORY_HPP

#include <memory>
#include <cstdint>

struct GBAHeader;

enum
{
    NUM_GAMEPAK_WAITSTATES = 3
};

// Memory Management Unit
class MMU
{
public:
    static std::unique_ptr<MMU>& Instance()
    {
        static std::unique_ptr<MMU> _instance(new MMU());
        return _instance;
    }

    void LoadROM(GBAHeader& header, FILE* rom);

    uint32_t Read32(uint32_t address);
    uint16_t Read16(uint32_t address);

    void Write32(uint32_t address, uint32_t value);
    void Write16(uint32_t address, uint16_t value);

private:
    MMU() { }

    uint8_t _bios[0x4000]; // 00000000-00003FFF   BIOS - System ROM         (16 KBytes)
    uint8_t _ewram[0x40000]; // 02000000 - 0203FFFF   WRAM - On - board Work RAM(256 KBytes) 2 Wait
    uint8_t _iwram[0x8000]; // 03000000-03007FFF   WRAM - On-chip Work RAM   (32 KBytes)

    // GamePak Memory
    uint8_t _pakROM[NUM_GAMEPAK_WAITSTATES][0x2000000]; // There are 3 Wait States (0 - 2), each are mirrors of the game ROM
};

#define sMemory MMU::Instance()
#endif