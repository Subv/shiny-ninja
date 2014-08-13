#ifndef MATH_HPP
#define MATH_HPP

namespace MathHelper
{
    bool CheckBit(uint8_t bit, uint32_t val)
    {
        return (val & (1 << bit));
    }

    bool CheckBits(uint32_t value, uint8_t start, uint8_t bits, uint32_t check)
    {
        return ((value >> start) & ((1 << bits) - 1)) == check;
    }
}
#endif