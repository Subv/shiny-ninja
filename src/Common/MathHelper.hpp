#ifndef MATH_HPP
#define MATH_HPP

#include <cstdint>

namespace MathHelper
{
    bool CheckBit(uint8_t bit, uint32_t val);

    bool CheckBits(uint32_t value, uint8_t start, uint8_t bits, uint32_t check);

    template <typename T>
    T RotateLeft(T val, uint8_t moves)
    {
        return (val << moves) | (val >> (sizeof(T) * CHAR_BIT - moves));
    }

    template<class T>
    T RotateRight(T x, uint8_t moves)
    {
        return (x >> moves) | (x << sizeof(T) * CHAR_BIT - moves);
    }
}
#endif