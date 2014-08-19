#include "PSRTransferInstructions.hpp"
#include "Common/MathHelper.hpp"
#include <sstream>

std::string ARM::MovePSRToRegisterInstruction::ToString() const
{
    std::stringstream stream;
    stream << "MRS R" << +GetDestinationRegister() << (GetPSRType() == PSRType::CPSR ? ", CPSR" : ", SPSR");
    return stream.str();
}

std::string ARM::MoveRegisterToPSRRegisterInstruction::ToString() const
{
    std::string psrSuffix = "_";

    if (MathHelper::CheckBit(GetFieldsMask(), 0))
        psrSuffix += "c";
    if (MathHelper::CheckBit(GetFieldsMask(), 1))
        psrSuffix += "x";
    if (MathHelper::CheckBit(GetFieldsMask(), 2))
        psrSuffix += "s";
    if (MathHelper::CheckBit(GetFieldsMask(), 3))
        psrSuffix += "f";

    std::string psr = (GetPSRType() == PSRType::CPSR ? "CPSR" : "SPSR") + psrSuffix;
    return "MSR " + psr + ", R" + std::to_string(GetSourceRegister());
}

std::string ARM::MoveRegisterToPSRImmediateInstruction::ToString() const
{
    std::string psrSuffix = "_";

    if (MathHelper::CheckBit(GetFieldsMask(), 0))
        psrSuffix += "c";
    if (MathHelper::CheckBit(GetFieldsMask(), 1))
        psrSuffix += "x";
    if (MathHelper::CheckBit(GetFieldsMask(), 2))
        psrSuffix += "s";
    if (MathHelper::CheckBit(GetFieldsMask(), 3))
        psrSuffix += "f";

    std::string psr = (GetPSRType() == PSRType::CPSR ? "CPSR" : "SPSR") + psrSuffix;
    return "MSR " + psr + ", #" + std::to_string(MathHelper::RotateRight(uint32_t(GetImmediateValue()), GetImmediateShift() << 1));
}
