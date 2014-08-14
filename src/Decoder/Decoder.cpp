#include "Decoder.hpp"
#include "Common/Instructions/ARM/BranchInstructions.hpp"
#include "Common/Instructions/ARM/DataProcessingInstructions.hpp"
#include "Common/Instructions/ARM/PSRTransferInstructions.hpp"

#include "Common/MathHelper.hpp"

using std::shared_ptr;

Decoder::Decoder()
{

}

shared_ptr<Instruction> Decoder::DecodeARM(uint32_t opcode)
{
    // Check for BLX and BX
    if (MathHelper::CheckBits(opcode, 8, 20, 0x12FFF))
        return shared_ptr<Instruction>(new ARM::BranchLinkExchangeRegisterInstruction(opcode));

    // Now check for B, BL and BLX_IMM
    if (MathHelper::CheckBits(opcode, 25, 3, 0x5))
    {
        if (MathHelper::CheckBits(opcode, 27, 4, 0xF))
            return shared_ptr<Instruction>(new ARM::BranchLinkExchangeImmediateInstruction(opcode));

        return shared_ptr<Instruction>(new ARM::BranchInstruction(opcode));
    }

    // PSR Transfer instructions
    // We have to check for these before we check for data processing instructions because these are a special case of the dataproc ones.
    if (MathHelper::CheckBits(opcode, 23, 5, 0x2) && MathHelper::CheckBits(opcode, 16, 6, 0xF) && MathHelper::CheckBits(opcode, 0, 12, 0)) // MRS Instruction
        return shared_ptr<Instruction>(new ARM::MovePSRToRegisterInstruction(opcode));

    if (MathHelper::CheckBits(opcode, 20, 2, 0x2) && MathHelper::CheckBits(opcode, 12, 4, 0xF))
    {
        if (MathHelper::CheckBits(opcode, 23, 5, 0x6)) // MSR_IMM
            return shared_ptr<Instruction>(new ARM::MoveRegisterToPSRImmediateInstruction(opcode));

        if (MathHelper::CheckBits(opcode, 4, 8, 0)) // MSR_REG
            return shared_ptr<Instruction>(new ARM::MoveRegisterToPSRRegisterInstruction(opcode));
    }

    // Check for data processing instructions
    if (MathHelper::CheckBits(opcode, 26, 2, 0))
        return shared_ptr<Instruction>(new ARM::DataProcessingInstruction(opcode));

    return shared_ptr<Instruction>(nullptr);
}

shared_ptr<Instruction> Decoder::DecodeThumb(uint16_t opcode)
{
    // Check for Add/Substract first, because first three bits also sign "Move Shifted Register"
    if (MathHelper::CheckBits(opcode, 11, 5, 0b00011))
        return shared_ptr<Instruction>(new Thumb::AddSubstractRegisterInstruction(opcode));

    if (MathHelper::CheckBits(opcode, 13, 2, 0b00011))
        return shared_ptr<Instruction>(new Thumb::MoveShiftedRegisterInstruction(opcode));

    return shared_ptr<Instruction>(nullptr);
}