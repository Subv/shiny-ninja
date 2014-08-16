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

    class MoveRegisterToPSRImmediateInstruction : public ARMInstruction
    {
    public:
        MoveRegisterToPSRImmediateInstruction(uint32_t instruction) : ARMInstruction(instruction) { }

        PSRType GetPSRType() { return PSRType((_instruction >> 22) & 1); }

        uint32_t GetOpcode() override { return ARMOpcodes::MSR; }

        std::string ToString() override;
        bool IsImmediate() override { return true; }

        uint8_t GetFieldsMask() { return (_instruction >> 16) & 0xF; }

        uint8_t GetImmediateValue() { return _instruction & 0xFF; }
        uint8_t GetImmediateShift() { return ((_instruction >> 8) & 0xF) << 2; }
    };

    class MoveRegisterToPSRRegisterInstruction : public ARMInstruction
    {
    public:
        MoveRegisterToPSRRegisterInstruction(uint32_t instruction) : ARMInstruction(instruction) { }

        PSRType GetPSRType() { return PSRType((_instruction >> 22) & 1); }

        uint32_t GetOpcode() override { return ARMOpcodes::MSR; }

        std::string ToString() override;

        uint8_t GetFieldsMask() { return (_instruction >> 16) & 0xF; }

        uint8_t GetSourceRegister() { return _instruction & 0xF; }
    };
}
#endif