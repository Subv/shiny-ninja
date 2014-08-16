#include "PSRTransferInstructions.hpp"
#include <sstream>

std::string ARM::MovePSRToRegisterInstruction::ToString()
{
    std::stringstream stream;
    stream << "MSR R" << GetDestinationRegister() << (GetPSRType() == PSRType::CPSR ? "CPSR" : "SPSR");
    return stream.str();
}

std::string ARM::MoveRegisterToPSRRegisterInstruction::ToString()
{
    return "MRS";
}

std::string ARM::MoveRegisterToPSRImmediateInstruction::ToString()
{
    return "MSR";
}
