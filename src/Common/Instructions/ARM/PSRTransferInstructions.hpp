#ifndef ARM_PSR_TRANSFER_INST_HPP
#define ARM_PSR_TRANSFER_INST_HPP

#include "Common/Instructions/ARMInstruction.hpp"

namespace ARM
{
    enum class PSRType
    {
        CPSR,
        SPSR
    };

    class MovePSRToRegisterInstruction : public ARMInstruction
    {
    public:
        MovePSRToRegisterInstruction(uint32_t instruction) : ARMInstruction(instruction) { }

        PSRType GetPSRType() { return PSRType((_instruction >> 22) & 1); }

        uint32_t GetOpcode() override { return ARMOpcodes::MRS; }
        
        std::string ToString() override;

        uint8_t GetDestinationRegister() { return (_instruction >> 12) & 0xF; }
    };

    class MoveRegisterToPSRInstruction : public ARMInstruction
    {
    public:
        MoveRegisterToPSRInstruction(uint32_t instruction) : ARMInstruction(instruction) { }

        PSRType GetPSRType() { return PSRType((_instruction >> 22) & 1); }

        uint32_t GetOpcode() override { return ARMOpcodes::MSR; }

        std::string ToString() override;

        bool Immediate() { return (_instruction >> 25) & 1; }
    };
}
#endif