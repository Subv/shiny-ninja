#include "BranchInstruction.hpp"

std::string ARM::BranchInstruction::ToString()
{
    return (Link() ? "BL " : "B ") + "PC + " + std::to_string(GetSignedOffset());
}

uint32_t ARM::BranchInstruction::GetOpcode()
{
    return Link() ? ARMOpcodes::BL : ARMOpcodes::B;
}

std::string ARM::BranchLinkExchangeImmediateInstruction::ToString()
{
    return "BLX PC + " + std::to_string(GetSignedOffset());
}

uint32_t ARM::BranchLinkExchangeImmediateInstruction::GetOpcode()
{
    return ARMOpcodes::BLX_IMM;
}

std::string ARM::BranchLinkExchangeRegisterInstruction::ToString()
{
    return (Link() ? "BLX " : "BX") + " R" + std::to_string(GetRegister());
}

uint32_t ARM::BranchLinkExchangeRegisterInstruction::GetOpcode()
{
    return Link() ? ARMOpcodes::BLX_REG : ARMOpcodes::BX;
}
