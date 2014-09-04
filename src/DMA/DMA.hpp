#ifndef DMA_HPP
#define DMA_HPP

#include <cstdint>

class CPU;
enum class InterruptTypes;

#define DMA0CNT_L 0x40000B8 // DMA 0 Word Count
#define DMA0CNT_H 0x40000BA // DMA0 Control Address
#define DMA0SAD 0x40000B0 // DMA0 Source Address
#define DMA0DAD 0x40000B4 // DMA0 Destination Address

#pragma pack(push, 1)
union DMAControl
{
    struct
    {
        uint8_t Enabled : 1;
        uint8_t IRQ : 1;
        uint8_t StartTiming : 2;
        uint8_t GamePakDRQ : 1;
        uint8_t TransferType : 1;
        uint8_t Repeat : 1;
        uint8_t SourceAddressControl : 2;
        uint8_t DestinationAddressControl : 2;
        uint8_t Unused : 5;
    } Data;

    uint16_t Full;

    DMAControl(uint16_t info) : Full(info) { }
};
#pragma pack(pop)

class DMA
{
public:
    enum StartType
    {
        Immediately,
        VBlank,
        HBlank,
        Special
    };

    enum Channel
    {
        DMA0,
        DMA1,
        DMA2,
        DMA3
    };

    DMA(CPU* cpu) : _cpu(cpu) { }

    void Step();
    void ProcessInterrupt(InterruptTypes type);


private:
    void InitiateTransfer(Channel channel, DMAControl control);

    // Helper functions
    static uint32_t GetDMAControlAddress(Channel channel) { return DMA0CNT_H + uint8_t(channel) * 0xC; }
    static uint32_t GetDMACountAddress(Channel channel) { return DMA0CNT_L + uint8_t(channel) * 0xC; }
    static uint32_t GetDMASourceAddress(Channel channel) { return DMA0SAD + uint8_t(channel) * 0xC; }
    static uint32_t GetDMADestinationAddress(Channel channel) { return DMA0DAD + uint8_t(channel) * 0xC; }

    CPU* _cpu;
};
#endif