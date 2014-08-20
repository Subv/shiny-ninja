#include "BranchExchangeInstruction.hpp"

#include "Common/Utilities.hpp"

#include <cstdint>
#include <sstream>

std::string Thumb::BranchExchangeInstruction::ToString() const
{
    std::stringstream stream;
    stream << "BX R" << GetTargetAddressRegister();
    return stream.str();
}

uint32_t Thumb::BranchExchangeInstruction::GetOpcode() const
{
    return ThumbOpcodes::BX;
}
