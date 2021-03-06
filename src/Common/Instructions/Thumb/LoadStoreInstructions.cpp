#include "LoadStoreInstructions.hpp"

#include "Common/Utilities.hpp"

#include <cstdint>
#include <sstream>
#include <bitset>

std::string Thumb::LoadFromLiteralStoreInstruction::ToString() const
{
    std::stringstream stream;
    stream << "LDR R" << GetDestinationRegister() << ", [PC, #0x";
    stream << std::hex << std::uppercase << GetImmediate() << "]";
    return stream.str();
}

std::string Thumb::LoadStoreRegisterOffsetInstruction::ToString() const
{
    std::stringstream stream;
    stream << Thumb::ToString(GetOpcode()) << " R" << GetDestinationRegister();
    stream << ", [R" << GetFirstOperand() << ", R" << GetSecondOperand() << "]";
    return stream.str();
}

uint32_t Thumb::LoadStoreRegisterOffsetInstruction::GetOpcode() const
{
    uint32_t opcode = MathHelper::GetBits(_instruction, 9, 3);
    if (opcode == 4)
        return ThumbOpcodes::LDR_2;
    if (opcode == 3)
        return ThumbOpcodes::LDRSB;
    if (opcode == 7)
        return ThumbOpcodes::LDRSH;
    if (opcode == 0)
        return ThumbOpcodes::STR_2;
    if (opcode == 2)
        return ThumbOpcodes::STRB_2;
    if (opcode == 1)
        return ThumbOpcodes::STRH_2;
    if (opcode == 5)
        return ThumbOpcodes::LDRH_2;
    return ThumbOpcodes::LDRB_2;
}

std::string Thumb::LoadStoreImmediateInstruction::ToString() const
{
    std::stringstream stream;
    stream << Thumb::ToString(GetOpcode()) << " R" << GetDestinationRegister();
    stream << ", [R" << GetBaseAddressRegister() << ", #0x" << std::hex << std::uppercase << GetImmediate() << "]";
    return stream.str();
}

uint32_t Thumb::LoadStoreImmediateInstruction::GetOpcode() const
{
    if (MathHelper::GetBits(_instruction, 13, 3) == 3)
    {
        if (IsLoad())
            return IsByte() ? ThumbOpcodes::LDRB_1 : ThumbOpcodes::LDR_1;
        return IsByte() ? ThumbOpcodes::STRB_1 : ThumbOpcodes::STR_1;
    }
    return IsLoad() ? ThumbOpcodes::LDRH_1 : ThumbOpcodes::STRH_1;
}

uint32_t Thumb::LoadStoreImmediateInstruction::GetImmediate() const
{
    uint32_t rawImm = MathHelper::GetBits(_instruction, 6, 5);
    if (GetOpcode() == ThumbOpcodes::LDR_1 || GetOpcode() == ThumbOpcodes::STR_1)
        rawImm <<= 2;
    else if (GetOpcode() == ThumbOpcodes::LDRH_1 || GetOpcode() == ThumbOpcodes::STRH_1)
        rawImm <<= 1;
    return rawImm;
}

std::string Thumb::LoadStoreStackInstruction::ToString() const
{
    std::stringstream stream;
    stream << Thumb::ToString(GetOpcode()) << " R" << GetDestinationRegister();
    stream << ", [SP, #0x" << std::hex << std::uppercase << GetRelativeOffset() << "]";
    return stream.str();
}

uint32_t Thumb::LoadStoreStackInstruction::GetOpcode() const
{
    if (MathHelper::CheckBit(_instruction, 11))
        return ThumbOpcodes::LDR_4;
    return ThumbOpcodes::STR_3;
}

std::string Thumb::LoadStoreMultipleInstruction::ToString() const
{
    std::stringstream stream;
    stream << Thumb::ToString(GetOpcode()) << " R" << GetRegister() << "!, {" ;

    std::bitset<8> registersBits(GetRegisterList());
    while (registersBits.count() > 0)
    {
        uint8_t bitIndex = 0;
        for (; bitIndex < 8 && !registersBits.test(bitIndex); ++bitIndex);

        stream << "R" << +bitIndex;
        registersBits[bitIndex] = false;
        if (registersBits.count() != 0)
            stream << ", ";
    }
    stream << "}";
    return stream.str();
}

uint32_t Thumb::LoadStoreMultipleInstruction::GetOpcode() const
{
    return IsLoad() ? ThumbOpcodes::LDMIA : ThumbOpcodes::STMIA;
}
