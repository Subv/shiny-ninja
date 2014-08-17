#include "MultiplyAccumulateInstructions.h"

#include "Common/Utilities.hpp"
#include <sstream>

uint8_t ARM::MultiplyAccumulateInstruction::GetThirdOperand() const
{
    Utilities::Assert(GetOpcode() == ARMOpcodes::MLA, "ARM::MultiplyAccumulateInstruction GetThirdOperand() called on MUL");
    return MathHelper::GetBits(_instruction, 12, 4);
}

uint32_t ARM::MultiplyAccumulateInstruction::GetOpcode() const
{
    if (MathHelper::CheckBit(_instruction, 21))
        return ARMOpcodes::MLA;

    return ARMOpcodes::MUL;
}

std::string ARM::MultiplyAccumulateInstruction::ToString() const
{
    std::stringstream opcode;
    if (GetOpcode() == ARMOpcodes::MLA)
        opcode << "MLA";
    else
        opcode << "MUL";

    opcode << " R" << GetDestinationRegister() << ", " << GetFirstOperand() << ", " << GetSecondOperand();

    if (GetOpcode() == ARMOpcodes::MLA)
        opcode << ", " << GetThirdOperand();

    return opcode.str();
}
