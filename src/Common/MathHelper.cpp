#include "MathHelper.hpp"

bool MathHelper::CheckBit(uint64_t val, uint8_t bit)
{
    return (val & uint64_t(1 << bit)) != 0;
}

bool MathHelper::CheckBits(uint32_t value, uint8_t start, uint8_t bitCount, uint32_t check)
{
    return ((value >> start) & ((1 << bitCount) - 1)) == check;
}

uint32_t MathHelper::GetBits(uint64_t value, uint8_t start, uint8_t bitCount)
{
    return ((value >> start) & ((1 << bitCount) - 1));
}