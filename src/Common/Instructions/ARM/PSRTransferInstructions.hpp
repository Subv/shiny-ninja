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

        PSRType GetPSRType() const { return PSRType((_instruction >> 22) & 1); }

        uint32_t GetOpcode() const override { return ARMOpcodes::MRS; }
        
        std::string ToString() const override;

        uint8_t GetDestinationRegister() const { return (_instruction >> 12) & 0xF; }
    };

    class MoveRegisterToPSRImmediateInstruction : public ARMInstruction
    {
    public:
        MoveRegisterToPSRImmediateInstruction(uint32_t instruction) : ARMInstruction(instruction) { }

        PSRType GetPSRType() const { return PSRType((_instruction >> 22) & 1); }

        uint32_t GetOpcode() const override { return ARMOpcodes::MSR; }

        std::string ToString() const override;
        bool IsImmediate() const override { return true; }

        uint8_t GetFieldsMask() const { return (_instruction >> 16) & 0xF; }

        uint8_t GetImmediateValue() const { return _instruction & 0xFF; }
        uint8_t GetImmediateShift() const { return ((_instruction >> 8) & 0xF) << 2; }
    };

    class MoveRegisterToPSRRegisterInstruction : public ARMInstruction
    {
    public:
        MoveRegisterToPSRRegisterInstruction(uint32_t instruction) : ARMInstruction(instruction) { }

        PSRType GetPSRType() const { return PSRType((_instruction >> 22) & 1); }

        uint32_t GetOpcode() const override { return ARMOpcodes::MSR; }

        std::string ToString() const override;

        uint8_t GetFieldsMask() const { return (_instruction >> 16) & 0xF; }

        uint8_t GetSourceRegister() const { return _instruction & 0xF; }
    };
}
#endif