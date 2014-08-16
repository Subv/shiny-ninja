#ifndef MATH_HPP
#define MATH_HPP

#include <cstdint>

namespace MathHelper
{
    bool CheckBit(uint8_t bit, uint32_t val);

    bool CheckBits(uint32_t value, uint8_t start, uint8_t bitCount, uint32_t check);

    uint32_t GetBits(uint32_t value, uint8_t start, uint8_t bitCount);

    template <typename T>
    inline T RotateLeft(T val, uint8_t moves)
    {
        return (val << moves) | (val >> (sizeof(T) * CHAR_BIT - moves));
    }

    template<typename T>
    inline T RotateRight(T x, uint8_t moves)
    {
        return (x >> moves) | (x << sizeof(T) * CHAR_BIT - moves);
    }
}
#endif