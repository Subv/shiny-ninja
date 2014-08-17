#ifndef ARM_BRANCH_INSTR_H
#define ARM_BRANCH_INSTR_H

#include "Common/Instructions/ARMInstruction.hpp"

#include "Common/MathHelper.hpp"

namespace ARM
{
    class BranchLinkExchangeImmediateInstruction : public ARMInstruction
    {
    public:
        BranchLinkExchangeImmediateInstruction(uint32_t instruction) : ARMInstruction(instruction) { }

        int32_t GetSignedOffset() const { return (_instruction & 0xFFFFFF) << 2; }
        uint8_t GetSecondBit() const { return MathHelper::GetBits(_instruction, 24, 1); }

        uint32_t GetOpcode() const override;
        bool IsImmediate() const override { return true; }

        std::string ToString() const override;
    };

    class BranchLinkExchangeRegisterInstruction : public ARMInstruction
    {
    public:
        BranchLinkExchangeRegisterInstruction(uint32_t instruction) : ARMInstruction(instruction) { }

        uint8_t GetRegister() const { return _instruction & 0xF; }

        uint32_t GetOpcode() const override;

        std::string ToString() const override;

        bool Link() const { return MathHelper::CheckBit(_instruction, 5); }
    };

    class BranchInstruction : public ARMInstruction
    {
    public:
        BranchInstruction(uint32_t instruction) : ARMInstruction(instruction) { }

        int32_t GetSignedOffset() const { return (_instruction & 0xFFFFFF) << 2; }

        uint32_t GetOpcode() const override;

        std::string ToString() const override;

        bool Link() const { return MathHelper::CheckBit(_instruction, 24); }
    };
}
#endif