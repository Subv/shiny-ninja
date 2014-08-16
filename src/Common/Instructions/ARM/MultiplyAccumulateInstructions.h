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

        bool SetConditionCodes() { return MathHelper::CheckBit(20, _instruction); }

        uint32_t GetOpcode() override;
        std::string ToString() override;

        uint8_t GetDestinationRegister() { return MathHelper::GetBits(_instruction, 16, 4); }
        uint8_t GetFirstOperand() { return MathHelper::GetBits(_instruction, 0, 4); }
        uint8_t GetSecondOperand() { return MathHelper::GetBits(_instruction, 8, 4); }
        uint8_t GetThirdOperand();
    };
}
#endif