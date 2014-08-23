#include "MultiplyAccumulateInstructions.hpp"

#include "Common/Utilities.hpp"
#include <sstream>

uint8_t ARM::MultiplyAccumulateInstruction::GetThirdOperand() const
{
    Utilities::Assert(GetOpcode() == ARMOpcodes::MLA, "ARM::MultiplyAccumulateInstruction GetThirdOperand() called on MUL");
    return MathHelper::GetBits(_instruction, 12, 4);
}

uint32_t ARM::MultiplyAccumulateInstruction::GetOpcode() const
{
    switch (MathHelper::GetBits(_instruction, 21, 3))
    {
        case 0:
            return ARMOpcodes::MUL;
        case 1:
            return ARMOpcodes::MLA;
        case 4:
            return ARMOpcodes::UMULL;
        case 5:
            return ARMOpcodes::UMLAL;
        case 6:
            return ARMOpcodes::SMULL;
        case 7:
            return ARMOpcodes::SMLAL;
        default:
            break;
    }
    Utilities::Assert(false, "Unrecognized opcode in ARM::MultiplyAccumulateInstruction");
    return 0;
}

std::string ARM::MultiplyAccumulateInstruction::ToString() const
{
    std::stringstream opcode;
    opcode << ARM::ToString(GetOpcode());

    opcode << " R" << GetDestinationRegisterHigh() << ", " << GetFirstOperand() << ", " << GetSecondOperand();

    if (GetOpcode() == ARMOpcodes::MLA)
        opcode << ", " << GetThirdOperand();

    return opcode.str();
}
