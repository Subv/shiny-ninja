#include "RegisterInstructions.hpp"

#include "Common/Utilities.hpp"

#include <cstdint>
#include <sstream>

std::string Thumb::ImmediateShiftInstruction::ToString() const
{
    std::stringstream stream;
    stream << Thumb::ToString(GetOpcode()) << " R" << GetDestinationRegister();
    stream << ", R" << GetSourceRegister() << ", #" << GetImmediateValue();
    return stream.str();
}

uint32_t Thumb::ImmediateShiftInstruction::GetOpcode() const
{
    switch (MathHelper::GetBits(_instruction, 11, 3))
    {
        case 000_b: return ThumbOpcodes::LSL;
        case 001_b: return ThumbOpcodes::LSR;
        case 010_b: return ThumbOpcodes::ASR;
        default:
            Utilities::Assert(false, "Thumb::ImmediateShiftInstruction: Invalid opcode");
            return 0;
    }
}

std::string Thumb::AddSubRegisterInstruction::ToString() const
{
    std::stringstream stream;
    stream << Thumb::ToString(GetOpcode()) << " R" << GetDestinationRegister();
    stream << ", R" << GetSourceRegister() << ", " << (IsImmediate() ? "#" : "R");
    stream << GetThirdOperand();
    return stream.str();
}

uint32_t Thumb::AddSubRegisterInstruction::GetOpcode() const
{
    if (MathHelper::GetBits(_instruction, 9, 1) == 0)
        return ThumbOpcodes::ADD;
    return ThumbOpcodes::SUB;
}

std::string Thumb::MovCmpAddSubImmediateInstruction::ToString() const
{
    std::stringstream stream;
    stream << Thumb::ToString(GetOpcode()) << " R" << GetDestinationRegister() << ", #" << GetImmediateValue();
    return stream.str();
}

uint32_t Thumb::MovCmpAddSubImmediateInstruction::GetOpcode() const
{
    switch (MathHelper::GetBits(_instruction, 11, 3))
    {
        case 100_b: return ThumbOpcodes::MOV; // 100xx
        case 101_b: return ThumbOpcodes::CMP; // 101xx
        case 110_b: return ThumbOpcodes::ADD; // 110xx
        case 111_b: return ThumbOpcodes::SUB; // 111xx
        default:
            Utilities::Assert(false, "Thumb::MovCmpAddSubImmediateInstruction: Invalid opcode");
            return 0;
    }
}