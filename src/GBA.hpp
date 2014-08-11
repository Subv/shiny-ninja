#ifndef GBA_HPP
#define GBA_HPP

#include <stdint.h>

struct GBAHeader
{
    uint32_t EntryPoint;
    uint8_t Logo[156];
    char Title[0xC];
    char GameCode[4];
    uint16_t MakerCode;
    uint8_t Is96h;
    uint8_t UnitCode;
    uint8_t DeviceCode;
    uint8_t Unused[7];
    uint8_t Version;
    uint8_t Complement;
    uint8_t Unused2[2];

    bool VerifyChecksum()
    {
        uint8_t check = 0;
        for (uint32_t i = 0xA0; i <= 0xBC; ++i)
            check -= *((uint8_t*)this + i);

        check = (check - 0x19) & 0xFF;
        
        return complement == check;
    }
};

#endif