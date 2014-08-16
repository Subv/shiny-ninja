#include "MultiplyAccumulateInstructions.h"

#include "Common/Utilities.hpp"

uint8_t ARM::MultiplyAccumulateInstruction::GetThirdOperand()
{
    Utilities::Assert(GetOpcode() == ARMOpcodes::MLA, "ARM::MultiplyAccumulateInstruction GetThirdOperand() called on MUL");
    return MathHelper::GetBits(_instruction, 12, 4);
}

uint32_t ARM::MultiplyAccumulateInstruction::GetOpcode()
{
    if (MathHelper::CheckBit(_instruction, 21))
        return ARMOpcodes::MLA;

    return ARMOpcodes::MUL;
}

std::string ARM::MultiplyAccumulateInstruction::ToString()
{
    std::string opcode = "";
    if (GetOpcode() == ARMOpcodes::MLA)
        opcode = "MLA";
    else
        opcode = "MUL";

    opcode += " R" + std::to_string(GetDestinationRegister()) + ", " + std::to_string(GetFirstOperand()) + ", " + std::to_string(GetSecondOperand());

    if (GetOpcode() == ARMOpcodes::MLA)
        opcode += ", " + std::to_string(GetThirdOperand());

    return opcode;
}
