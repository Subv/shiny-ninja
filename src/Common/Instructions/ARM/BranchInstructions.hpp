#ifndef ARM_BRANCH_INSTR_H
#define ARM_BRANCH_INSTR_H

#include "Common/Instructions/ARMInstruction.hpp"

namespace ARM
{
    class BranchLinkExchangeImmediateInstruction : public ARMInstruction
    {
    public:
        BranchLinkExchangeImmediateInstruction(uint32_t instruction) : ARMInstruction(instruction) { }

        int32_t GetSignedOffset() { return (_instruction & 0xFFFFFF) << 2; }

        uint32_t GetOpcode() override;
        bool IsImmediate() override { return true; }

        std::string ToString() override;
    };

    class BranchLinkExchangeRegisterInstruction : public ARMInstruction
    {
    public:
        BranchLinkExchangeRegisterInstruction(uint32_t instruction) : ARMInstruction(instruction) { }

        uint8_t GetRegister() { return _instruction & 0xF; }

        bool Link() { return (_instruction >> 5) & 0x1; }

        uint32_t GetOpcode() override;

        std::string ToString() override;
    };

    class BranchInstruction : public ARMInstruction
    {
    public:
        BranchInstruction(uint32_t instruction) : ARMInstruction(instruction) { }

        int32_t GetSignedOffset() { return (_instruction & 0xFFFFFF) << 2; }

        uint32_t GetOpcode() override;

        std::string ToString() override;

        bool Link() { return (_instruction >> 24) & 0x1; }
    };
}
#endif