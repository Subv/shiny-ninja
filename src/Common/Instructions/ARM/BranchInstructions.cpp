#include "BranchInstructions.hpp"
#include "Common/Utilities.hpp"
#include <sstream>
#include <string>

std::string ARM::BranchInstruction::ToString()
{
    std::stringstream stream;
    stream << (Link() ? "BL PC + " : "B PC + ") << (GetSignedOffset() + 8);
    return stream.str();
}

uint32_t ARM::BranchInstruction::GetOpcode()
{
    return Link() ? ARMOpcodes::BL : ARMOpcodes::B;
}

std::string ARM::BranchLinkExchangeImmediateInstruction::ToString()
{
    std::stringstream stream;
    stream << "BLX PC + " << (GetSignedOffset() + 8);
    return stream.str();
}

uint32_t ARM::BranchLinkExchangeImmediateInstruction::GetOpcode()
{
    return ARMOpcodes::BLX_IMM;
}

std::string ARM::BranchLinkExchangeRegisterInstruction::ToString()
{
    std::stringstream stream;
    stream << (Link() ? "BLX R" : "BX R") << GetRegister() << " + 8";
    return stream.str();
}

uint32_t ARM::BranchLinkExchangeRegisterInstruction::GetOpcode()
{
    return Link() ? ARMOpcodes::BLX_REG : ARMOpcodes::BX;
}
