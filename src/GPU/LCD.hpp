#ifndef LCD_HPP
#define LCD_HPP

#include <cstdint>

#pragma pack(push, 1)
union DISPCNT_STRUCT
{
    struct Bits
    {
        uint8_t BgMode : 3;
        uint8_t CgbMode : 1;
        uint8_t DisplayFrameSelect : 1;
        uint8_t HBlankIntervalFree : 1;
        uint8_t ObjVramMapping : 1;
        uint8_t ForcedBlank : 1;
        uint8_t BG0 : 1;
        uint8_t BG1 : 1;
        uint8_t BG2 : 1;
        uint8_t BG3 : 1;
        uint8_t OBJ : 1;
        uint8_t DisplayFlagWindow0 : 1;
        uint8_t DisplayFlagWindow1 : 1;
        uint8_t DisplayFlagObj : 1;
    };

    uint16_t Full;
};

union DISPSTAT_STRUCT
{
    struct Bits
    {
        uint8_t VBlank : 1;
        uint8_t HBlank : 1;
        uint8_t VCounter : 1;
        uint8_t VBlankIrqEnable : 1;
        uint8_t HBlankIrqEnable : 1;
        uint8_t VCounterIrqEnable : 1;
        uint8_t Unused : 2;
        uint8_t LYC : 8;
    };

    uint16_t Full;
};
#pragma pack(pop)

class LCD
{

};

#endif
