#include "BranchInstructions.hpp"

std::string ARM::BranchInstruction::ToString()
{
    return (Link() ? "BL PC + " : "B PC + ") + std::to_string(GetSignedOffset() + 8);
}

uint32_t ARM::BranchInstruction::GetOpcode()
{
    return Link() ? ARMOpcodes::BL : ARMOpcodes::B;
}

std::string ARM::BranchLinkExchangeImmediateInstruction::ToString()
{
    return "BLX PC + " + std::to_string(GetSignedOffset() + 8);
}

uint32_t ARM::BranchLinkExchangeImmediateInstruction::GetOpcode()
{
    return ARMOpcodes::BLX_IMM;
}

std::string ARM::BranchLinkExchangeRegisterInstruction::ToString()
{
    return (Link() ? "BLX R" : "BX R") + std::to_string(GetRegister()) + " + 8";
}

uint32_t ARM::BranchLinkExchangeRegisterInstruction::GetOpcode()
{
    return Link() ? ARMOpcodes::BLX_REG : ARMOpcodes::BX;
}
