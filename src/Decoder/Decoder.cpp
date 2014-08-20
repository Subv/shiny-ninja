#include "Decoder.hpp"
#include "Common/Instructions/ARM/BranchInstructions.hpp"
#include "Common/Instructions/ARM/DataProcessingInstructions.hpp"
#include "Common/Instructions/ARM/PSRTransferInstructions.hpp"
#include "Common/Instructions/ARM/MultiplyAccumulateInstructions.hpp"
#include "Common/Instructions/ARM/LoadStoreInstructions.hpp"

#include "Common/Instructions/Thumb/DataProcessingInstructions.hpp"
#include "Common/Instructions/Thumb/BranchExchangeInstruction.hpp"
#include "Common/Instructions/Thumb/LoadStoreInstructions.hpp"

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

    if (MathHelper::CheckBits(opcode, 25, 3, 0) && MathHelper::CheckBit(opcode, 7) && MathHelper::CheckBit(opcode, 4))
        return shared_ptr<Instruction>(new ARM::MiscellaneousLoadStoreInstruction(opcode));

    // Check for data processing instructions
    if (MathHelper::CheckBits(opcode, 26, 2, 0))
        return shared_ptr<Instruction>(new ARM::DataProcessingInstruction(opcode));

    // Check for multiply / multiply accumulate instructions
    if (MathHelper::CheckBits(opcode, 22, 6, 0) && MathHelper::CheckBits(opcode, 4, 4, 9))
        return shared_ptr<Instruction>(new ARM::MultiplyAccumulateInstruction(opcode));

    // We group the LDR/STR and LDM/STM instructions in a single class
    if (MathHelper::CheckBits(opcode, 26, 2, 1) || MathHelper::CheckBits(opcode, 26, 2, 2))
        return shared_ptr<Instruction>(new ARM::LoadStoreInstruction(opcode));

    return shared_ptr<Instruction>(nullptr);
}

shared_ptr<Instruction> Decoder::DecodeThumb(uint16_t opcode)
{
    if (MathHelper::GetBits(opcode, 13, 3) == 0) // 000 (LSR, LSL, ASR, ADD SUB REG)
    {
        if (MathHelper::GetBits(opcode, 11, 2) <= 2)
            return shared_ptr<Instruction>(new Thumb::ImmediateShiftInstruction(opcode));
        return shared_ptr<Instruction>(new Thumb::AddSubInstruction(opcode));
    }

    if (MathHelper::GetBits(opcode, 13, 3) == 1) // ADD SUB CMP MOV IMM
        return shared_ptr<Instruction>(new Thumb::AddSubCmpMovImmInstruction(opcode));

    if (MathHelper::GetBits(opcode, 10, 6) == 16) // DataProc
        return shared_ptr<Instruction>(new Thumb::DataProcessingInstruction(opcode));

    if (MathHelper::GetBits(opcode, 10, 6) == 17) // SpecialDataProc and Branch
    {
        if (MathHelper::GetBits(opcode, 8, 2) == 3)
            return shared_ptr<Instruction>(new Thumb::BranchExchangeInstruction(opcode));

        return shared_ptr<Instruction>(new Thumb::SpecialDataProcessingInstruction(opcode));
    }

    if (MathHelper::GetBits(opcode, 11, 5) == 9) // LDR literal store
        return shared_ptr<Instruction>(new Thumb::LoadFromLiteralStoreInstruction(opcode));

    if (MathHelper::GetBits(opcode, 12, 4) == 5) // Load/Store Register
        return shared_ptr<Instruction>(new Thumb::LoadStoreRegisterOffsetInstruction(opcode));

    if (MathHelper::GetBits(opcode, 13, 3) == 3 // Load/Store Word/Byte Immediate
        || MathHelper::GetBits(opcode, 12, 4) == 8) // Load Store Halfword Immediate
        return shared_ptr<Instruction>(new Thumb::LoadStoreImmediateInstruction(opcode));

    if (MathHelper::GetBits(opcode, 12, 4) == 9) // Load/Store to/from stack
        return shared_ptr<Instruction>(new Thumb::LoadStoreStackInstruction(opcode));

    if (MathHelper::GetBits(opcode, 12, 4) == 11) // Misc.
        return shared_ptr<Instruction>(nullptr); // NYI

    if (MathHelper::GetBits(opcode, 12, 4) == 12) // Load/Store Multiple
        return shared_ptr<Instruction>(new Thumb::LoadStoreMultipleInstruction(opcode));

    return shared_ptr<Instruction>(nullptr);
}
