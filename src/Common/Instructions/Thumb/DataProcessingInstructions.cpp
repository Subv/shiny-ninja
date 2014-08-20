#include "DataProcessingInstructions.hpp"

#include "Common/Utilities.hpp"

#include <cstdint>
#include <sstream>

std::string Thumb::ImmediateShiftInstruction::ToString() const
{
    std::stringstream stream;
    stream << Thumb::ToString(GetOpcode()) << " R" << GetDestinationRegister();
    stream << ", R" << GetSourceRegister() << ", #" << GetOffset();
    return stream.str();
}

uint32_t Thumb::ImmediateShiftInstruction::GetOpcode() const
{
    switch (MathHelper::GetBits(_instruction, 11, 2))
    {
        case 0: return ThumbOpcodes::LSL_1;
        case 1: return ThumbOpcodes::LSR_1;
        case 2: return ThumbOpcodes::ASR_1;
    }
    return 0;
}

std::string Thumb::AddSubInstruction::ToString() const
{
    std::stringstream stream;
    stream << Thumb::ToString(GetOpcode()) << " R" << GetDestinationRegister();
    stream << ", R" << GetSourceRegister() << ", ";
    stream << (IsImmediate() ? "#" : "R") << GetThirdOperand();
    return stream.str();
}

uint32_t Thumb::AddSubInstruction::GetOpcode() const
{
    if (!MathHelper::CheckBit(_instruction, 9))
        return ThumbOpcodes::ADD_1;
    return ThumbOpcodes::SUB_1;
}

std::string Thumb::AddSubCmpMovImmInstruction::ToString() const
{
    std::stringstream stream;
    stream << Thumb::ToString(GetOpcode()) << " R" << GetDestinationRegister();
    stream << ", #0x" << std::hex << std::uppercase << GetImmediate();
    return stream.str();
}

uint32_t Thumb::AddSubCmpMovImmInstruction::GetOpcode() const
{
    switch (MathHelper::GetBits(_instruction, 11, 2))
    {
        case 0: return ThumbOpcodes::MOV_1;
        case 1: return ThumbOpcodes::CMP_1;
        case 2: return ThumbOpcodes::ADD_2;
    }
    return ThumbOpcodes::SUB_2;
}

std::string Thumb::DataProcessingInstruction::ToString() const
{
    std::stringstream stream;
    stream << Thumb::ToString(GetOpcode()) << " R";
    stream << GetDestinationRegister() << ", R" << GetSourceRegister();
    return stream.str();
}

uint32_t Thumb::DataProcessingInstruction::GetOpcode() const
{
    switch (MathHelper::GetBits(_instruction, 6, 4))
    {
        case 0: return ThumbOpcodes::AND;
        case 1: return ThumbOpcodes::EOR;
        case 2: return ThumbOpcodes::LSL_2;
        case 3: return ThumbOpcodes::LSR_2;
        case 4: return ThumbOpcodes::ASR_2;
        case 5: return ThumbOpcodes::ADC;
        case 6: return ThumbOpcodes::SBC;
        case 7: return ThumbOpcodes::ROR;
        case 8: return ThumbOpcodes::TST;
        case 9: return ThumbOpcodes::NEG;
        case 10: return ThumbOpcodes::CMP_2;
        case 11: return ThumbOpcodes::CMN;
        case 12: return ThumbOpcodes::ORR;
        case 13: return ThumbOpcodes::MUL;
        case 14: return ThumbOpcodes::BIC;
        // case 15: return ThumbOpcodes::MVN;
    }
    return ThumbOpcodes::MVN;
}

std::string Thumb::SpecialDataProcessingInstruction::ToString() const
{
    std::stringstream stream;
    stream << Thumb::ToString(GetOpcode());
    stream << " R" << GetDestinationRegister();
    stream << ", R" << GetFirstDataRegister();
    return stream.str();
}

uint32_t Thumb::SpecialDataProcessingInstruction::GetOpcode() const
{
    switch (MathHelper::GetBits(_instruction, 8, 2))
    {
        case 0: return ThumbOpcodes::ADD_4;
        case 1: return ThumbOpcodes::CMP_3;
        case 2: return ThumbOpcodes::MOV_3;
    }
    Utilities::Assert(false, "SpecialDataProcessInstructions: Opcode 3 should be handled by BX instructions!");
}


