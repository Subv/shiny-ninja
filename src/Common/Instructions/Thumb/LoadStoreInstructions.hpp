#ifndef THUMB_LSDR_INSTR_H
#define THUMB_LSDR_INSTR_H

#include "Common/Instructions/ThumbInstruction.hpp"
#include "Common/MathHelper.hpp"

namespace Thumb
{
    class LoadFromLiteralStoreInstruction : public ThumbInstruction
    {
    public:
        LoadFromLiteralStoreInstruction(uint16_t op) : ThumbInstruction(op) { }

        uint32_t GetOpcode() const override { return ThumbOpcodes::LDR_3; }
        std::string ToString() const override;
        bool IsImmediate() const override { return true; }

        uint32_t GetDestinationRegister() const { return MathHelper::GetBits(_instruction, 8, 3); }
        uint32_t GetImmediate() const { return MathHelper::GetBits(_instruction, 0, 8) << 2; }
    };

    class LoadStoreRegisterOffsetInstruction : public ThumbInstruction
    {
    public:
        LoadStoreRegisterOffsetInstruction(uint16_t op) : ThumbInstruction(op) { }

        uint32_t GetOpcode() const override;
        std::string ToString() const override;

        bool IsLoad() const { return MathHelper::CheckBit(_instruction, 11); }
        bool IsSignedOperation() const { return MathHelper::CheckBit(_instruction, 0); }

        uint32_t GetDestinationRegister() const { return MathHelper::GetBits(_instruction, 0, 3); }
        uint32_t GetFirstOperand() const { return MathHelper::GetBits(_instruction, 3, 3); }
        uint32_t GetSecondOperand() const { return MathHelper::GetBits(_instruction, 6, 3); }
    };

    class LoadStoreImmediateInstruction : public ThumbInstruction
    {
    public:
        LoadStoreImmediateInstruction(uint16_t op) : ThumbInstruction(op) { }

        bool IsImmediate() const override { return true; }

        uint32_t GetOpcode() const override;
        std::string ToString() const override;

        bool IsByte() const { return MathHelper::CheckBit(_instruction, 12); }
        bool IsLoad() const { return MathHelper::CheckBit(_instruction, 11); }

        uint32_t GetDestinationRegister() const { return MathHelper::GetBits(_instruction, 0, 3); }
        uint32_t GetBaseAddressRegister() const { return MathHelper::GetBits(_instruction, 3, 3); }
        uint32_t GetImmediate() const { return MathHelper::GetBits(_instruction, 6, 5); }
    };

    class LoadStoreStackInstruction : public ThumbInstruction
    {
    public:
        LoadStoreStackInstruction(uint16_t op) : ThumbInstruction(op) { }

        uint32_t GetOpcode() const override;
        std::string ToString() const override;

        bool IsStackPointerOperation() const { return MathHelper::CheckBit(_instruction, 11); }
        uint32_t GetDestinationRegister() const { return MathHelper::GetBits(_instruction, 8, 3); }
        uint32_t GetRelativeOffset() const { return MathHelper::GetBits(_instruction, 0, 8); }
    };

    class LoadStoreMultipleInstruction : public ThumbInstruction
    {
    public:
        LoadStoreMultipleInstruction(uint16_t op) : ThumbInstruction(op) { }

        uint32_t GetOpcode() const override;
        std::string ToString() const override;

        bool IsLoad() const { return MathHelper::CheckBit(_instruction, 11); }

        uint32_t GetRegister() const { return MathHelper::GetBits(_instruction, 8, 3); }
        uint32_t GetRegisterList() const { return MathHelper::GetBits(_instruction, 0, 8); }
    };
}

#endif // THUMB_LSDR_INSTR_H
