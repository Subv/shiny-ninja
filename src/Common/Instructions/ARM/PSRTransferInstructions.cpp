#include "PSRTransferInstructions.hpp"

std::string ARM::MovePSRToRegisterInstruction::ToString()
{
    return "MSR R" + std::to_string(GetDestinationRegister()) + (GetPSRType() == PSRType::CPSR ? "CPSR" : "SPSR");
}

std::string ARM::MoveRegisterToPSRRegisterInstruction::ToString()
{
    return "MRS";
}

std::string ARM::MoveRegisterToPSRImmediateInstruction::ToString()
{
    return "MSR";
}
