#include "MathHelper.hpp"

inline bool MathHelper::CheckBit(uint8_t bit, uint32_t val)
{
    return (val & (1 << bit));
}

inline bool MathHelper::CheckBits(uint32_t value, uint8_t start, uint8_t bitCount, uint32_t check)
{
    return ((value >> start) & ((1 << bits) - 1)) == check;
}

inline uint32_t MathHelper::GetBits(uint32_t value, uint8_t start, uint8_t bitCount)
{
    return ((value >> start) & ((1 << bits) - 1));
}
