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
    // TODO: STUB

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

