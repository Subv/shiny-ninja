#include "GPU.hpp"

// http://www.cs.rit.edu/~tjh8300/CowBite/CowBiteSpec.htm
// http://problemkaputt.de/gbatek.htm

uint32_t GPU::ReadInt32(uint32_t offset) final
{

}

void GPU::ExtractColorValues(uint16_t input, uint8_t& red, uint8_t& green, uint8_t& blue) final
{
    // F E D C  B A 9 8  7 6 5 4  3 2 1 0
    // X B B B  B B G G  G G G R  R R R R
    blue = (input >> 10) & 0b11111;
    green = (input >> 5) & 0b11111;
    red = input & 0b11111;
}

uint16_t GPU::ReadInt16(uint32_t offset) final
{
    switch (offset >> 24)
    {
        case 0x05: // BG/OBJ Palette RAM
            // This is mirrored every 0x400 bytes.
            return *(uint16_t*)(&_obj[(offset & 0xFFF) % 0x400]);
        case 0x06: // VRAM
            // Bytes 0x06010000 - 0x06017FFF is mirrored from 0x06018000 - 0x0601FFFF.
            // The entire region from 0x06000000 - 0x06020000 is in turn mirrored every
            // 0x20000 bytes from 0x06000000 - 0x06FFFFFF.
            //! TODO: Doublecheck this, this may crash if reading after 0x06018000.
            return *(uint16_t*)(&_vram[(offset & 0xFFFFF) % 0x20000]);
        case 0x07: // OAM
            // This is mirrored every 0x400 bytes.
            return *(uint16_t*)(&_oam[(offset & 0xFFF) % 0x400]);
    }
    return 0;
}

uint32_t GPU::ReadInt32(uint32_t offset) final
{
    return ReadInt16(offset) | (ReadInt16(offset + 4) << 16);
}
