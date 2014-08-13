#include "PSRTransferInstructions.hpp"

std::string ARM::MovePSRToRegisterInstruction::ToString()
{
    return "MSR R" + std::to_string(GetDestinationRegister()) + (GetPSRType() == PSRType::CPSR ? "CPSR" : "SPSR");
}

