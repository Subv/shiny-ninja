#ifndef ARM_DATAPROC_INST_H
#define ARM_DATAPROC_INST_H

#include "Common/Instructions/ARMInstruction.hpp"

namespace ARM
{
    enum class ShiftType
    {
        LSL,
        LSR,
        ASR,
        ROR
    };

    class DataProcessingInstruction : public ARMInstruction
    {
    public:
        DataProcessingInstruction(uint32_t instruction) : ARMInstruction(instruction) { }

        uint32_t GetOpcode() const override;
        std::string ToString() const override;

        bool IsImmediate() const override { return (_instruction >> 25) & 1; }
        bool SetConditionCodes() const { return (_instruction >> 20) & 1; }

        uint8_t GetFirstOperand() const { return (_instruction >> 16) & 0xF; } // The first operand is always a register

        uint8_t GetDestinationRegister() const { return (_instruction >> 12) & 0xF; }

        // Returns wether or not this opcode should affect the overflow (V) flag of the CPU's CPSR
        bool AffectsOverflow() const;

        ShiftType GetShiftType() const;

        bool HasFirstOperand() const;
        bool HasDestinationRegister() const;

        uint8_t GetSecondOperand() const;

        // If Immediate()
        uint8_t GetShiftImmediate() const;
        uint32_t GetShiftedSecondOperandImmediate() const; // Returns GetSecondOperand() with the specified shift by GetShiftImmediate() applied

        // If not Immediate()
        bool ShiftByRegister() const;
        uint8_t GetShiftRegisterOrImmediate() const;
    };
}
#endif