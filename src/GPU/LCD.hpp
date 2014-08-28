#ifndef LCD_HPP
#define LCD_HPP

#include <cstdint>

class LCDAdapter
{
public:
    virtual ~LCDAdapter() { }

    virtual uint8_t* GetDataArray() const = 0;

private:
    uint8_t pixelData[4 * 256 * 192];
};

#endif
