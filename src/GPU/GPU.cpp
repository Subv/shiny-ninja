#include "GPU.hpp"
#include "CPU/CPU.hpp"

#include <iostream>

// http://www.cs.rit.edu/~tjh8300/CowBite/CowBiteSpec.htm
// http://problemkaputt.de/gbatek.htm

void GPU::ExtractColorValues(uint16_t input, uint8_t& red, uint8_t& green, uint8_t& blue)
{
    // F E D C  B A 9 8  7 6 5 4  3 2 1 0
    // X B B B  B B G G  G G G R  R R R R
    blue = (input >> 10) & 0x1F;
    green = (input >> 5) & 0x1F;
    red = input & 0x1F;
}

bool GPU::ReadBit(uint32_t offset, uint8_t bitIndex)
{
    uint8_t rebasedOffset = bitIndex / 8;
    return ReadUInt8(offset + rebasedOffset) & (1 << (bitIndex % 8));
}

uint8_t GPU::ReadUInt8(uint32_t offset)
{
    switch (offset >> 24)
    {
        case 0x05: // BG/OBJ Palette RAM
            // This is mirrored every 0x400 bytes.
            return _obj[(offset & 0xFFF) % 0x400];
        case 0x06: // VRAM
            // Bytes 0x06010000 - 0x06017FFF is mirrored from 0x06018000 - 0x0601FFFF.
            // The entire region from 0x06000000 - 0x06020000 is in turn mirrored every
            // 0x20000 bytes from 0x06000000 - 0x06FFFFFF.
            //! TODO: Doublecheck this, this may crash if reading after 0x06018000.
            return _vram[(offset & 0xFFFFF) % 0x20000];
        case 0x07: // OAM
            // This is mirrored every 0x400 bytes.
            return _oam[(offset & 0xFFF) % 0x400];
        default:
            return _cpu->GetMemory()->ReadUInt8(offset);
    }
}

uint16_t GPU::ReadUInt16(uint32_t offset)
{
    return (ReadUInt8(offset) << 8) | ReadUInt8(offset + 1);
}

uint32_t GPU::ReadUInt32(uint32_t offset)
{
    return (ReadUInt16(offset) << 16) | ReadUInt16(offset + 2);
}

void GPU::WriteUInt8(uint32_t offset, uint8_t value)
{
    switch (offset >> 24)
    {
        case 0x05: // BG/OBJ Palette RAM
            // This is mirrored every 0x400 bytes.
            _obj[(offset & 0xFFF) % 0x400] = value;
            break;
        case 0x06: // VRAM
            // Bytes 0x06010000 - 0x06017FFF is mirrored from 0x06018000 - 0x0601FFFF.
            // The entire region from 0x06000000 - 0x06020000 is in turn mirrored every
            // 0x20000 bytes from 0x06000000 - 0x06FFFFFF.
            //! TODO: Doublecheck this, this may crash if reading after 0x06018000.
            _vram[(offset & 0xFFFFF) % 0x20000] = value;
            break;
        case 0x07: // OAM
            // This is mirrored every 0x400 bytes.
            _oam[(offset & 0xFFF) % 0x400] = value;
            break;
        default:
            _cpu->GetMemory()->WriteUInt8(offset, value);
            break;
    }
}

void GPU::WriteUInt32(uint32_t offset, uint32_t value)
{
    WriteUInt16(offset, (value >> 16) & 0xFFFF);
    WriteUInt16(offset + 2, value & 0xFFFF);
}

void GPU::WriteUInt16(uint32_t offset, uint16_t value)
{
    WriteUInt8(offset, (value >> 8) & 0xFF);
    WriteUInt8(offset + 1, value & 0xFF);
}

/*
 * Designed to only be used on Memory Mapped I/O Registers
 */
void GPU::WriteBit(uint32_t offset, uint8_t bitIndex, bool isSet)
{
    /*uint8_t rebasedOffset = bitIndex / 8;
    uint8_t bitValue = (1 << (bitIndex % 8));

    if (isSet)
        _ioram[offset % 0x400 + rebasedOffset] |= bitValue;
    else
        _ioram[offset % 0x400 + rebasedOffset] &= ~bitValue;*/
}

// GPU logic
void GPU::Step(uint32_t cycles)
{
/*
 * The LCD has a refresh rate of about 59.73 hz, with each refresh consisting
 * of a vertical draw period (when the GBA is drawing the screen) followed
 * by a vertical blank period (when nothing is being drawn). The vertical draw
 * and vertical blank periods are further subdivided into horizontal draw and
 * blank periods.
 *
 * This location stores the current y location of the LCD hardware. It is
 * incremented as the lines are drawn. The 160 lines of display are followed
 * by 68 lines of Vblank period, before the whole thing starts again for the
 * next frame. Waiting for this register to reach 160 is one way to synchronize
 * a program to 60Hz.
 */
    uint8_t vCount = ReadUInt8(VCOUNT);
    if (vCount == ReadUInt8(DISPSTAT + 1))
    {
        // Checking bits 8-15 of u16 DISPSTAT against VCOUNT
        // If they are equal, V-COUNTER (#2) of DISPSTAT is set, and
        // if #5 is set, an IRQ is requested.
        WriteBit(DISPSTAT, 2, true);
        if (ReadBit(DISPSTAT, 5))
            WriteBit(InterruptRequestFlags, 2, true); // Request VCounter IRQ
    }

    WriteBit(DISPSTAT, 0, vCount >= VDRAW_DURATION && vCount <= VDRAW_DURATION + VBLANK_DURATION);
    WriteBit(DISPSTAT, 1, cycles >= HDRAW_DURATION && cycles <= HDRAW_DURATION + HBLANK_DURATION);
}
