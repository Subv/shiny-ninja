#include "Utilities.hpp"
#include "MathHelper.hpp"

#include <iostream>

void Utilities::Assert(bool condition, const char* message)
{
    if (!condition)
    {
        std::cerr << std::endl << "ASSERTION FAILURE:" << std::endl;
        std::cerr << " --> what(): " << message << std::endl << std::endl;
        std::exit(EXIT_FAILURE);
    }
}


uint32_t GeneralPurposeRegister::GetBits(uint8_t firstBitIndex, uint32_t bitCount)
{
    return MathHelper::GetBits(Value, firstBitIndex, bitCount);
}
