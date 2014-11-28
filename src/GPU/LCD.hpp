#ifndef LCD_HPP
#define LCD_HPP

#include <cstdint>

class LCDAdapter
{
public:
    virtual ~LCDAdapter() { }

    uint8_t* GetDataArray() { return pixelData; }
    virtual void DrawHorizontal(uint8_t line) = 0;
    virtual void EndFrame() = 0;

private:
    uint16_t pixelData[0x9600]; // 240x160 screen. This holds the composed screen (16 bit color per pixel), taking into account the video mode and all active backgrounds
};

#endif
