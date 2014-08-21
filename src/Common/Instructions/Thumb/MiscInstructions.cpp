#include "MiscInstructions.hpp"

#include <cstdint>
#include <sstream>
#include <bitset>

std::string Thumb::StackOperation::ToString() const
{
    std::stringstream stream;
    stream << Thumb::ToString(GetOpcode()) << " ";
    std::bitset<9> registersSet(GetRegisterMask());
    stream << "{";
    uint8_t bitSetCount = registersSet.count();
    while (bitSetCount > 0)
    {
        uint8_t bitIndex = 0;
        for (; bitIndex < 9 && !registersSet.test(bitIndex); ++bitIndex);

        if (bitIndex == 8)
            stream << (IsPopOperand() ? "PC" : "LR");
        else
            stream << "R" << +bitIndex;
        registersSet[bitIndex] = false;
        --bitSetCount;
        if (bitSetCount != 0)
            stream << ", ";
    }
    stream << "}";
    return stream.str();
}

uint32_t Thumb::StackOperation::GetOpcode() const
{
    return IsPopOperand() ? ThumbOpcodes::POP : ThumbOpcodes::PUSH;
}
