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

std::string Thumb::BranchInstruction::ToString() const
{
    std::stringstream stream;
    stream << Thumb::ToString(GetOpcode()) << " PC ";
    if (GetBranchOffset() > 0)
        stream << "+ " << GetBranchOffset();
    else
        stream << "- " << -GetBranchOffset();
    return stream.str();
}

uint32_t Thumb::BranchInstruction::GetOpcode() const
{
    if (IsConditionalBranch())
        return ThumbOpcodes::B_CONDITIONAL;
    return ThumbOpcodes::B_UNCONDITIONAL;
}

std::string Thumb::LongBranchLinkInstruction::ToString() const
{
    std::stringstream stream;
    stream << Thumb::ToString(GetOpcode()) << " " << GetOffset();
    return stream.str();
}
