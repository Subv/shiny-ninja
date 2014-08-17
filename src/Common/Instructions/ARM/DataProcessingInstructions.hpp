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

        uint32_t GetOpcode() override;
        std::string ToString() override;

        bool IsImmediate() override { return (_instruction >> 25) & 1; }
        bool SetConditionCodes() { return (_instruction >> 20) & 1; }

        uint8_t GetFirstOperand() { return (_instruction >> 16) & 0xF; } // The first operand is always a register

        uint8_t GetDestinationRegister() { return (_instruction >> 12) & 0xF; }

        ShiftType GetShiftType();

        bool HasFirstOperand();
        bool HasDestinationRegister();

        // If Immediate()
        uint8_t GetSecondOperand();
        uint8_t GetShiftImmediate();

        // If not Immediate()
        bool ShiftByRegister();
        uint8_t GetShiftRegisterOrImmediate();
    };
}
#endif