#include "DMA.hpp"
#include "CPU/CPU.hpp"

void DMA::Step()
{
    // Check for pending transfers
    for (uint8_t channel = 0; channel <= 3; ++channel)
    {
        DMAControl control = DMAControl(_cpu->GetMemory()->ReadUInt16(GetDMAControlAddress(Channel(channel))));
        
        // Ignore disabled channels
        if (!control.Data.Enabled)
            continue;

        if (control.Data.StartTiming == StartType::Immediately)
            InitiateTransfer(Channel(channel), control);
    }
}

void DMA::InitiateTransfer(Channel channel, DMAControl control)
{
    uint32_t sourceAddress = _cpu->GetMemory()->ReadUInt32(GetDMASourceAddress(channel));
    uint32_t destinationAddress = _cpu->GetMemory()->ReadUInt32(GetDMADestinationAddress(channel));

    // The source/destination address is 27 bits for channel 0 and 28 bits for channels 1-3
    if (channel == Channel::DMA0)
    {
        sourceAddress &= 0x7FFFFFF;
        destinationAddress &= 0x7FFFFFF;
    }
    else
    {
        sourceAddress &= 0xFFFFFFF;
        destinationAddress &= 0xFFFFFFF;
    }

    // The DMA transfer size is only 14 bits
    uint16_t size = _cpu->GetMemory()->ReadUInt16(GetDMACountAddress(channel)) & 0x3FFF;


    // If we are not supposed to repeat the transfer, disable it when finished
    if (!control.Data.Repeat)
    {
        control.Data.Enabled = 0;
        _cpu->GetMemory()->WriteUInt16(GetDMAControlAddress(channel), control.Full);
    }

    // Request an interrupt at the end of the transfer if needed
    if (control.Data.IRQ)
        _cpu->RequestInterrupt(InterruptTypes(uint8_t(InterruptTypes::DMA0) + channel));
}

void DMA::ProcessInterrupt(InterruptTypes type)
{
    // Check if any enabled DMA transfer channel is scheduled to run when this interrupt is triggered

    for (uint8_t channel = 0; channel <= 3; ++channel)
    {
        DMAControl control = DMAControl(_cpu->GetMemory()->ReadUInt16(GetDMAControlAddress(Channel(channel))));

        // Ignore disabled channels
        if (!control.Data.Enabled)
            continue;

        if (control.Data.StartTiming == StartType::VBlank && type == InterruptTypes::VBlank ||
            control.Data.StartTiming == StartType::HBlank && type == InterruptTypes::HBlank)
            InitiateTransfer(Channel(channel), control);
    }
}

