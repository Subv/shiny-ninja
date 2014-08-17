#ifndef ARM_MULTIPLY_ACC_INST_H
#define ARM_MULTIPLY_ACC_INST_H

#include "Common/Instructions/ARMInstruction.hpp"
#include "Common/MathHelper.hpp"

namespace ARM
{
    class MultiplyAccumulateInstruction : public ARMInstruction
    {
    public:
        MultiplyAccumulateInstruction(uint32_t instruction) : ARMInstruction(instruction) { }

        bool SetConditionCodes() const { return MathHelper::CheckBit(20, _instruction); }

        uint32_t GetOpcode() const override;
        std::string ToString() const override;

        uint8_t GetDestinationRegister() const { return MathHelper::GetBits(_instruction, 16, 4); }
        uint8_t GetFirstOperand() const { return MathHelper::GetBits(_instruction, 0, 4); }
        uint8_t GetSecondOperand() const { return MathHelper::GetBits(_instruction, 8, 4); }
        uint8_t GetThirdOperand() const;
    };
}
#endif