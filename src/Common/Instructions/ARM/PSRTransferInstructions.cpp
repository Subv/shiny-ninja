#include "PSRTransferInstructions.hpp"
#include <sstream>

std::string ARM::MovePSRToRegisterInstruction::ToString() const
{
    std::stringstream stream;
    stream << "MRS R" << GetDestinationRegister() << (GetPSRType() == PSRType::CPSR ? "CPSR" : "SPSR");
    return stream.str();
}

std::string ARM::MoveRegisterToPSRRegisterInstruction::ToString() const
{
    return "MSR";
}

std::string ARM::MoveRegisterToPSRImmediateInstruction::ToString() const
{
    return "MSR";
}
