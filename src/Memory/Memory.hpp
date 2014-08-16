#ifndef MEMORY_HPP
#define MEMORY_HPP

#include <memory>
#include <cstdint>

struct GBAHeader;
struct CPU;

enum WaitStates
{
    WAIT_STATE_0 = 0,
    WAIT_STATE_1 = 1,
    WAIT_STATE_2 = 2,
    NUM_WAIT_STATES = 3
};

// Memory Management Unit
class MMU final
{
public:
    MMU(std::shared_ptr<CPU> arm);

    void LoadROM(GBAHeader& header, FILE* rom);

    uint32_t ReadInt32(uint32_t address);
    uint16_t ReadInt16(uint32_t address);

    void WriteInt32(uint32_t address, uint32_t value);
    void WriteInt16(uint32_t address, uint16_t value);

    bool IsInBios() const { return _inBios; }

private:

    uint8_t _bios[0x4000];   // 00000000 - 00003FFF   BIOS - System ROM         (16 KBytes)
    uint8_t _ewram[0x40000]; // 02000000 - 0203FFFF   WRAM - On-board Work RAM  (256 KBytes) 2 Wait
    uint8_t _iwram[0x8000];  // 03000000 - 03007FFF   WRAM - On-chip Work RAM   (32 KBytes)
    uint8_t _vram[0x18000];  // 06000000 - 06017FFF   VRAM - Video RAM          (96 KBytes)

    uint8_t _pakROM[NUM_WAIT_STATES][0x2000000];
    uint8_t _sram[0x10000];  // 0E000000 - 0E00FFFF   Game Pak SRAM    (max 64 KBytes) - 8bit Bus width

    std::shared_ptr<CPU> _cpu;
    bool _inBios; // Constantly set to false for now, we do not have a BIOS.
};

#endif
