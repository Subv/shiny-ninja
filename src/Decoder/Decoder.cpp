#include "Decoder.hpp"
#include "Common/Instructions/ARM/BranchInstructions.hpp"
#include "Common/Instructions/ARM/DataProcessingInstructions.hpp"
#include "Common/Instructions/ARM/PSRTransferInstructions.hpp"
#include "Common/Instructions/ARM/MultiplyAccumulateInstructions.h"
#include "Common/Instructions/ARM/LoadStoreInstructions.h"

#include "Common/Instructions/Thumb/RegisterInstructions.hpp"

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

    // Check for multiply / multiply accumulate instructions
    if (MathHelper::CheckBits(opcode, 22, 6, 0) && MathHelper::CheckBits(opcode, 4, 4, 9))
        return shared_ptr<Instruction>(new ARM::MultiplyAccumulateInstruction(opcode));

    if (MathHelper::CheckBits(opcode, 26, 2, 1))
        return shared_ptr<Instruction>(new ARM::LoadStoreInstruction(opcode));

    return shared_ptr<Instruction>(nullptr);
}

shared_ptr<Instruction> Decoder::DecodeThumb(uint16_t opcode)
{
    if (MathHelper::CheckBits(opcode, 14, 2, 00_b))
    {
        // LSL 000xxb, LSR 001xxb, ASR 010xxb
        if (MathHelper::GetBits(opcode, 9, 5) & 01111_b)
            return shared_ptr<Instruction>(new Thumb::ImmediateShiftInstruction(opcode));
        
        if (MathHelper::GetBits(opcode, 9, 5) & 01111_b)
            return shared_ptr<Instruction>(new Thumb::AddSubRegisterInstruction(opcode));
            
        return shared_ptr<Instruction>(new Thumb::MovCmpAddSubImmediateInstruction(opcode));
    }
    
    /*uint32_t identifier = MathHelper::GetBits(opcode, 10, 4);
    
    if (MathHelper::CheckBits(opcode, 14, 2, 01_b))
    {
        if (identifier & 0000_b) // (01)0000
            return shared_ptr<Instruction>(new Thumb::DataProcessingInstructions(opcode));

        if (identifier & 0001_b) // (01)0001
            return shared_ptr<Instruction>(new Thumb::SpecialDataAndBranchExchangeInstruction(opcode));
        
        // (01)001x Load from Literal Pool, see LDR (literal) on page A6-90
        if (identifier & 0010_b)
            return shared_ptr<Instruction>(new Thumb::LoadStoreInstruction(opcode)); 
        
        // (01)01xx Load/store single data item on page A5-9
        // (01)1xxx Load/store single data item on page A5-9
        // (10)0xxx Load/store single data item on page A5-9
        return shared_ptr<Instruction>(new Thumb::LoadStoreInstruction(opcode));
    }
    
    if (MathHelper::CheckBits(opcode, 14, 2, 10_b))
    {
        // (10)100x Generate PC-relative address, see ADR on page A6-30
        if (identifier & 1001_b))
            return shared_ptr<Instruction>(new Thumb::ADR(opcode));
        
        // (10)101x Generate SP-relative address, see ADD (SP plus immediate) on page A6-26
        if (identifier & 1011_b)
            return shared_ptr<Instruction>(new Thumb::ADD(opcode));

        // (10)11xx Miscellaneous 16-bit instructions on page A5-10
        if (identifier & 1111_b)
            return shared_ptr<Instruction>(new Thumb::MiscInstructions(opcode));

        return shared_ptr<Instruction>(nullptr);
    }
    
    // At this point 14-15 is forced to be 0b11.
    
    // (11)000x Store multiple registers, see STM / STMIA / STMEA on page A6-218
    if (identifier & 0001_b)
        return shared_ptr<Instruction>(new Thumb::StoreRegistersInstruction(opcode));
        
    // (11)001x Load multiple registers, see LDM / LDMIA / LDMFD on page A6-84
    if (identifier & 0011_b)
        return shared_ptr<Instruction>(new Thumb::LoadRegistersInstruction(opcode));

    // (11)01xx Conditional branch, and supervisor call on page A5-12
    if (identifier & 0111_b)
        return shared_ptr<Instruction>(new Thumb::CondBranchSupervisorInstruction(opcode));
    
    if (identifier & 1001_b)
        return shared_ptr<Instruction>(new Thumb::UnconditionalBranchInstruction(opcode));
*/
    return shared_ptr<Instruction>(nullptr);
}