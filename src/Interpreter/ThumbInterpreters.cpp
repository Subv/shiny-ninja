#include "Interpreter.hpp"

#include "CPU/CPU.hpp"
#include "Memory/Memory.hpp"

#include "Common/Instructions/ThumbInstruction.hpp"

#include "Common/Instructions/Thumb/MiscInstructions.hpp"
#include "Common/Instructions/Thumb/DataProcessingInstructions.hpp"
#include "Common/Instructions/Thumb/BranchExchangeInstruction.hpp"
#include "Common/Instructions/Thumb/LoadStoreInstructions.hpp"

#include "Common/MathHelper.hpp"
#include "Common/Utilities.hpp"

// Sections: A7.1.49, A7.1.50
//! TODO: CP15_reg1_Ubit vodoo - what is that?
void Interpreter::HandleThumbStackOperationInstruction(std::shared_ptr<ThumbInstruction> instruction)
{
    auto stackOperation = std::static_pointer_cast<Thumb::StackOperation>(instruction);

    uint32_t registersSet = stackOperation->GetRegisterMask(); // Includes R (bit 7)
    if (stackOperation->IsPopOperand())
    {
        // MemoryAccess(B-bit, E-bit)
        uint32_t startAddress = _cpu->GetRegister(SP);
        uint32_t endAddress = startAddress + 4 * MathHelper::NumberOfSetBits(registersSet);
        uint32_t address = startAddress;

        for (uint8_t i = 0; i < 8; ++i)
        {
            if ((registersSet & (1 << i)) == 0)
                continue;
            _cpu->GetRegister(i) = _cpu->GetMemory()->ReadUInt32(address);
            address += 4;
        }

        if ((registersSet & (1 << 8)) != 0)
        {
            uint32_t value = _cpu->GetMemory()->ReadUInt32(address);
            _cpu->GetRegister(PC) = value & 0xFFFFFFFE;
            address += 4;
        }

        Utilities::Assert(endAddress == address, "");
        _cpu->GetRegister(SP) = endAddress;
    }
    else
    {
        uint32_t startAddress = _cpu->GetRegister(SP) - 4 * MathHelper::NumberOfSetBits(registersSet);
        uint32_t endAddress = _cpu->GetRegister(SP) - 4;
        uint32_t address = startAddress;
        for (uint8_t i = 0; i < 8; ++i)
        {
            if ((registersSet & (1 << i)) == 0)
                continue;
             _cpu->GetMemory()->WriteUInt32(address, _cpu->GetRegister(i));
            address += 4;
        }

        if ((registersSet & (1 << 8)) != 0)
        {
            _cpu->GetMemory()->WriteUInt32(address, _cpu->GetRegister(LR));
            address += 4;
        }

        Utilities::Assert(endAddress == address - 4, "");
        _cpu->GetRegister(SP) = startAddress;

        // if (CP15_reg1_Ubit == 1) /* ARMv6 */
        //   if Shared(address then /* from ARMv6 */
        //     physical_address = TLB(address
        //     ClearExclusiveByAddress(physical_address, size)
    }
}

// Sections: A7.1.38, A7.1.40, A7.1.12
void Interpreter::HandleThumbImmediateShiftInstruction(std::shared_ptr<ThumbInstruction> instruction)
{
    auto shiftInstr = std::static_pointer_cast<Thumb::ImmediateShiftInstruction>(instruction);

    GeneralPurposeRegister& Rm = _cpu->GetRegister(shiftInstr->GetSourceRegister());
    GeneralPurposeRegister& Rd = _cpu->GetRegister(shiftInstr->GetDestinationRegister());
    uint32_t Imm = shiftInstr->GetOffset();

    switch (shiftInstr->GetOpcode())
    {
        case Thumb::ThumbOpcodes::LSL_1:
        {
            Rd = Rm << Imm;
            if (shiftInstr->GetOffset() != 0)
                _cpu->GetCurrentStatusFlags().C = Rm[32 - Imm];
            break;
        }
        case Thumb::ThumbOpcodes::LSR_1:
        {
            _cpu->GetCurrentStatusFlags().C = Rm[Imm == 0 ? 31 : (Imm - 1)];
            Rd = Imm == 0 ? 0 : (Rm << Imm);
            break;
        }
        case Thumb::ThumbOpcodes::ASR_1:
        {
            _cpu->GetCurrentStatusFlags().C = Rm[Imm == 0 ? 31 : (Imm - 1)];
            Rd = (Imm == 0) ? (Rm[31] == 0 ? 0 : 0xFFFFFFFF) : (int32_t(Rm) >> 5);
            break;
        }
    }
    _cpu->GetCurrentStatusFlags().N = Rd[31];
    _cpu->GetCurrentStatusFlags().Z = Rd == 0;
}

// Sections: A7.1.3, A7.1.5, A7.1.65, A7.1.67
void Interpreter::HandleThumbAddSubImmRegInstruction(std::shared_ptr<ThumbInstruction> instruction)
{
    auto instr = std::static_pointer_cast<Thumb::AddSubInstruction>(instruction);
    GeneralPurposeRegister& Rd = _cpu->GetRegister(instr->GetDestinationRegister());
    GeneralPurposeRegister& Rn = _cpu->GetRegister(instr->GetSourceRegister());
    uint32_t Rm = instr->IsImmediate() ?
                instr->GetThirdOperand() : // Imm
                _cpu->GetRegister(instr->GetThirdOperand()); // Rm

    switch (instr->GetOpcode())
    {
        case Thumb::ThumbOpcodes::ADD_1:
        case Thumb::ThumbOpcodes::ADD_3:
            Rd = Rn + Rm;
            _cpu->GetCurrentStatusFlags().C = Rd.Carry;
            break;
        case Thumb::ThumbOpcodes::SUB_1:
        case Thumb::ThumbOpcodes::SUB_3:
            Rd = Rn - Rm;
            _cpu->GetCurrentStatusFlags().C = !Rd.Borrow;
            break;

    }
    _cpu->GetCurrentStatusFlags().V = Rd.Overflow;
    _cpu->GetCurrentStatusFlags().N = Rd[31];
    _cpu->GetCurrentStatusFlags().Z = Rd == 0;
}

// Sections: A7.1.42, A7.1.21, A7.1.4, A7.1.66
void Interpreter::HandleThumbAddCmpMovSubImmediateInstruction(std::shared_ptr<ThumbInstruction> instruction)
{
    auto realInstr = std::static_pointer_cast<Thumb::AddSubCmpMovImmInstruction>(instruction);

    GeneralPurposeRegister& Rd = _cpu->GetRegister(realInstr->GetDestinationRegister());
    uint32_t Imm = realInstr->GetImmediate();
    switch (realInstr->GetOpcode())
    {
        case Thumb::ThumbOpcodes::MOV_1:
            Rd = Imm;
            _cpu->GetCurrentStatusFlags().N = Rd[31];
            _cpu->GetCurrentStatusFlags().Z = Rd == 0;
            break;
        case Thumb::ThumbOpcodes::CMP_1: // Rd is actually Rn here
        {
            int64_t aluOut = int64_t(Rd) - Imm;
            _cpu->GetCurrentStatusFlags().N = (aluOut >> 31) & 1;
            _cpu->GetCurrentStatusFlags().Z = aluOut == 0;
            _cpu->GetCurrentStatusFlags().C = !MathHelper::BorrowFrom(Rd, Imm);
            _cpu->GetCurrentStatusFlags().V = MathHelper::Overflow<uint32_t>(aluOut);
            break;
        }
        case Thumb::ThumbOpcodes::ADD_2:
            Rd += Imm;
            _cpu->GetCurrentStatusFlags().N = Rd[31];
            _cpu->GetCurrentStatusFlags().Z = Rd == 0;
            _cpu->GetCurrentStatusFlags().C = Rd.Carry;
            _cpu->GetCurrentStatusFlags().V = Rd.Overflow;
            break;
        case Thumb::ThumbOpcodes::SUB_2:
            Rd -= Imm;
            _cpu->GetCurrentStatusFlags().N = Rd[31];
            _cpu->GetCurrentStatusFlags().Z = Rd == 0;
            _cpu->GetCurrentStatusFlags().C = !Rd.Borrow;
            _cpu->GetCurrentStatusFlags().V = Rd.Overflow;
            break;

    }
}

// Sections: A7.1.10, A7.1.26, A7.1.39, A7.1.41, A7.1.12, A7.1.2, A7.1.55
//           A7.1.54, A7.1.72, A7.1.20, A7.1.22, A7.1.48, A7.1.45, A7.1.15
void Interpreter::HandleThumbDataProcessingInstruction(std::shared_ptr<ThumbInstruction> instruction)
{
    auto realInstr = std::static_pointer_cast<Thumb::DataProcessingInstruction>(instruction);
    GeneralPurposeRegister& Rd = _cpu->GetRegister(realInstr->GetFirstOperand());
    GeneralPurposeRegister& Rm = _cpu->GetRegister(realInstr->GetSecondOperand());

    uint32_t opcode = realInstr->GetOpcode();

    switch (opcode)
    {
        case Thumb::ThumbOpcodes::AND:
            Rd &= Rm;
            _cpu->GetCurrentStatusFlags().N = Rd[31];
            _cpu->GetCurrentStatusFlags().Z = Rd == 0;
            break;
        case Thumb::ThumbOpcodes::EOR:
            Rd ^= Rm;
            _cpu->GetCurrentStatusFlags().N = Rd[31];
            _cpu->GetCurrentStatusFlags().Z = Rd == 0;
            break;
        case Thumb::ThumbOpcodes::LSL_2: // Rm = Rs here
        {
            uint8_t RsLoByte = Rm.GetBits(0, 8);
            if (RsLoByte != 0)
            {
               if (RsLoByte <= 32)
                   _cpu->GetCurrentStatusFlags().C = Rd[32 - RsLoByte];
               else // if (RsLoByte > 32)
                    _cpu->GetCurrentStatusFlags().C = 0;
               Rd = RsLoByte < 32 ? (Rd << RsLoByte) : 0;
            }
            _cpu->GetCurrentStatusFlags().N = Rd[31];
            _cpu->GetCurrentStatusFlags().Z = Rd == 0;
            break;
        }
        case Thumb::ThumbOpcodes::LSR_2: // Rm = Rs here
        {
            uint8_t RsLoByte = Rm.GetBits(0, 8);
            if (RsLoByte != 0)
            {
               if (RsLoByte < 32)
                   _cpu->GetCurrentStatusFlags().C = Rd[RsLoByte - 1];
               else // if (RsLoByte >= 32)
                    _cpu->GetCurrentStatusFlags().C = (RsLoByte == 32) ? Rd[31] : 0;
               Rd = RsLoByte < 32 ? (Rd >> RsLoByte) : 0;
            }
            _cpu->GetCurrentStatusFlags().N = Rd[31];
            _cpu->GetCurrentStatusFlags().Z = Rd == 0;
            break;
        }
        case Thumb::ThumbOpcodes::ASR_2: // Rm = Rs here
        {
            uint8_t RsLoByte = Rm.GetBits(0, 8);
            if (RsLoByte != 0)
            {
               if (RsLoByte < 32)
               {
                   _cpu->GetCurrentStatusFlags().C = Rd[RsLoByte - 1];
                   Rd = (int32_t(Rd) >> RsLoByte);
               }
               else // if (RsLoByte >= 32)
               {
                    _cpu->GetCurrentStatusFlags().C = Rd[31];
                    Rd = Rd[31] == 0 ? 0 : 0xFFFFFFFF;
               }
            }
            _cpu->GetCurrentStatusFlags().N = Rd[31];
            _cpu->GetCurrentStatusFlags().Z = Rd == 0;
            break;
        }
        case Thumb::ThumbOpcodes::ADC:
        {
            uint64_t dirtySum = Rd;
            if (!_cpu->GetCurrentStatusFlags().C)
            {
                dirtySum += Rm;
                _cpu->GetCurrentStatusFlags().C = dirtySum < Rm;
            }
            else
            {
                dirtySum += Rm + 1;
                _cpu->GetCurrentStatusFlags().C = dirtySum <= Rm;
            }
            Rd = dirtySum;
            _cpu->GetCurrentStatusFlags().N = Rd[31];
            _cpu->GetCurrentStatusFlags().Z = Rd == 0;
            _cpu->GetCurrentStatusFlags().V = MathHelper::Overflow<uint32_t>(dirtySum);
            break;
        }
        case Thumb::ThumbOpcodes::SBC:
        {
            Rd -= Rm + !_cpu->GetCurrentStatusFlags().C;
            _cpu->GetCurrentStatusFlags().N = Rd[31];
            _cpu->GetCurrentStatusFlags().Z = Rd == 0;
            _cpu->GetCurrentStatusFlags().C = !Rd.Borrow;
            _cpu->GetCurrentStatusFlags().V = Rd.Overflow;
            break;
        }
        case Thumb::ThumbOpcodes::ROR: // Rm = Rs here
        {
            uint8_t RsLoByte = Rm.GetBits(0, 8);
            if (RsLoByte != 0)
            {
                if (Rm.GetBits(0, 5) == 0)
                    _cpu->GetCurrentStatusFlags().C = Rd[31];
                else // if (Rm.GetBits(0, 5) > 0) { Rs[4:0] }
                {
                    // ROR is stored in the LSB.
                    _cpu->GetCurrentStatusFlags().C = Rd[Rm.GetBits(0, 4) - 1];
                    Rd = MathHelper::RotateRight(Rd, Rm.GetBits(0, 4));
                }
            }
            _cpu->GetCurrentStatusFlags().N = Rd[31];
            _cpu->GetCurrentStatusFlags().Z = Rd == 0;
            break;
        }
        case Thumb::ThumbOpcodes::TST:
        {
            int32_t aluOut = Rd & Rm;
            _cpu->GetCurrentStatusFlags().N = aluOut & (1 << 31);
            _cpu->GetCurrentStatusFlags().Z = aluOut == 0;
            break;
        }
        case Thumb::ThumbOpcodes::NEG:
        {
            Rd = 0 - Rm;
            _cpu->GetCurrentStatusFlags().N = Rd[31];
            _cpu->GetCurrentStatusFlags().Z = Rd == 0;
            _cpu->GetCurrentStatusFlags().C = !Rd.Borrow; //! TODO Doublecheck
            _cpu->GetCurrentStatusFlags().Z = Rd.Overflow;
            break;
        }
        case Thumb::ThumbOpcodes::CMN: // Rd is actually Rn here
        case Thumb::ThumbOpcodes::CMP_2: // Rd is actually Rn here
        {
            int32_t aluOut = (opcode == Thumb::ThumbOpcodes::CMN) ? (Rd + Rm) : (Rd - Rm);
            _cpu->GetCurrentStatusFlags().N = aluOut & (1 << 31);
            _cpu->GetCurrentStatusFlags().Z = aluOut == 0;
            _cpu->GetCurrentStatusFlags().V = MathHelper::Overflow<uint32_t>(aluOut);
            if (opcode == Thumb::ThumbOpcodes::CMP_2)
                _cpu->GetCurrentStatusFlags().C = !MathHelper::BorrowFrom(Rd, Rm);
            else
                _cpu->GetCurrentStatusFlags().C = MathHelper::CarryFrom(Rd, Rm);
            break;
        }
        case Thumb::ThumbOpcodes::ORR:
            Rd |= Rm;
            _cpu->GetCurrentStatusFlags().N = Rd[31];
            _cpu->GetCurrentStatusFlags().Z = Rd == 0;
            break;
        case Thumb::ThumbOpcodes::MUL:
            Rd *= Rm;
            _cpu->GetCurrentStatusFlags().N = Rd[31];
            _cpu->GetCurrentStatusFlags().Z = Rd == 0;
            break;
        case Thumb::ThumbOpcodes::BIC:
            Rd &= ~Rm;
            _cpu->GetCurrentStatusFlags().N = Rd[31];
            _cpu->GetCurrentStatusFlags().Z = Rd == 0;
            break;
    }
}

// Sections: A7.1.6, A7.1.23, A7.1.44
//! TODO: UNPREDICTABLE if MOV and both lo registers.
void Interpreter::HandleThumbSpecialDataProcessingInstruction(std::shared_ptr<ThumbInstruction> instruction)
{
    auto realInstr = std::static_pointer_cast<Thumb::SpecialDataProcessingInstruction>(instruction);

    GeneralPurposeRegister& Rd = _cpu->GetRegister(realInstr->GetDestinationRegister());
    GeneralPurposeRegister& Rm = _cpu->GetRegister(realInstr->GetFirstDataRegister());

    switch (realInstr->GetOpcode())
    {
        case Thumb::ThumbOpcodes::ADD_4:
            if (realInstr->HiOperandFlagDestination() == 0
                && realInstr->HiOperandFlagSource() == 0) // UNPREDICTABLE
            {
                // Fsck.
            }
            Rd += Rm;
            break;
        case Thumb::ThumbOpcodes::CMP_3:
        {
            uint32_t aluOut = Rd - Rm;
            _cpu->GetCurrentStatusFlags().N = aluOut & (1 << 31);
            _cpu->GetCurrentStatusFlags().Z = aluOut == 0;
            _cpu->GetCurrentStatusFlags().C = !MathHelper::BorrowFrom(Rd, Rm);
            _cpu->GetCurrentStatusFlags().V = MathHelper::Overflow<uint32_t>(aluOut);
            break;
        }
        case Thumb::ThumbOpcodes::MOV_3:
            if (realInstr->HiOperandFlagDestination() == 0
                && realInstr->HiOperandFlagSource() == 0) // UNPREDICTABLE
            {
                // Fsck.
            }
            Rd = Rm;
            break;
        default:
            break;
    }
}

// Sections: A7.1.19, A7.1.18
//! TODO Fix BX (special case if Rm == R15!)
void Interpreter::HandleThumbBranchExchangeInstruction(std::shared_ptr<ThumbInstruction> instruction)
{
    auto realInstr = std::static_pointer_cast<Thumb::BranchExchangeInstruction>(instruction);

    GeneralPurposeRegister Rm = _cpu->GetRegister(realInstr->GetTargetAddressRegister());

    // Register 15 can be specified for <Rm>. If this is done, R15
    // is read as normal for Thumb code, that is, it is the address
    // of the BX instruction itself plus 4. If the BX instruction is
    // at a word-aligned address, this results in a branch to the
    // next word, executing in ARM state. However, if the
    // BX instruction is not at a word-aligned address, this means
    // that the results of the instruction are UNPREDICTABLE
    // (because the value read for R15 has bits[1:0]==0b10).
    if (realInstr->GetTargetAddressRegister() == PC)
    {
        // Increment the PC register value by 2 because the real GBA CPU uses Pre-fetching
        Rm += 2;
        // Do stuff.
    }

    // If Rm[1:0] == 0b10, the result is UNPREDICTABLE, as branches
    // to non word-aligned addresses are impossible in ARM state.
    if (Rm[0] == 0 && Rm[1] == 1)
    {
        // Do stuff.
    }

    _cpu->SetInstructionSet(Rm[0] ? InstructionSet::Thumb : InstructionSet::ARM);
    _cpu->GetRegister(PC) = Rm.GetBits(1, 31) << 1;
}

// Section: A7.1.30
void Interpreter::HandleThumbLiteralPoolLoadInstruction(std::shared_ptr<ThumbInstruction> instruction)
{
    auto instr = std::static_pointer_cast<Thumb::LoadFromLiteralStoreInstruction>(instruction);
    uint32_t address = ((_cpu->GetRegister(PC) + 2) & 0xFFFFFFFC) + instr->GetImmediate();
    _cpu->GetRegister(instr->GetDestinationRegister()) = _cpu->GetMemory()->ReadUInt32(address);
}

// Sections: A7.1.29, A7.1.36, A7.1.37, A7.1.59, A7.1.62, A7.1.64, A7.1.35, A7.1.33
//! TODO: Some of those have CP15_reg1_Ubit vodoo - Again, no idea what this is
//! Also get rid of SignExtend - it just needs signed memory Read...
void Interpreter::HandleThumbLoadStoreRegisterOffsetInstruction(std::shared_ptr<ThumbInstruction> instruction)
{
    auto instr = std::static_pointer_cast<Thumb::LoadStoreRegisterOffsetInstruction>(instruction);

    GeneralPurposeRegister Rd = _cpu->GetRegister(instr->GetDestinationRegister());
    GeneralPurposeRegister Rn = _cpu->GetRegister(instr->GetFirstOperand());
    GeneralPurposeRegister Rm = _cpu->GetRegister(instr->GetSecondOperand());
    
    if (instr->GetDestinationRegister() == PC)
        Rd += 2;
    if (instr->GetFirstOperand() == PC)
        Rn += 2;
    if (instr->GetSecondOperand() == PC)
        Rm += 2;

    switch (instr->GetOpcode())
    {
        case Thumb::ThumbOpcodes::LDR_2:
            Rd = _cpu->GetMemory()->ReadUInt32(Rn + Rm);
            break;
        case Thumb::ThumbOpcodes::LDRSB:
            Rd = SignExtend(_cpu->GetMemory()->ReadUInt8(Rn + Rm));
            break;
        case Thumb::ThumbOpcodes::LDRSH:
            Rd = SignExtend(_cpu->GetMemory()->ReadUInt16(Rn + Rm));
            break;
        case Thumb::ThumbOpcodes::STR_2:
            _cpu->GetMemory()->WriteUInt32(Rn + Rm, Rd);
            break;
        case Thumb::ThumbOpcodes::STRB_2:
            _cpu->GetMemory()->WriteUInt8(Rn + Rm, Rd.GetBits(0, 8));
            break;
        case Thumb::ThumbOpcodes::STRH_2:
            _cpu->GetMemory()->WriteUInt16(Rn + Rm, Rd.GetBits(0, 16));
            break;
        case Thumb::ThumbOpcodes::LDRH_2:
            Rd = _cpu->GetMemory()->ReadUInt16(Rn + Rm); // ZeroPromote
            break;
        case Thumb::ThumbOpcodes::LDRB_2:
            Rd = _cpu->GetMemory()->ReadUInt8(Rn + Rm); // ZeroPromote
            break;
        default:
            break;
    }

    _cpu->GetRegister(instr->GetDestinationRegister()) = Rd;
}

// Not verified
void Interpreter::HandleThumbLoadStoreImmediateOffsetInstruction(std::shared_ptr<ThumbInstruction> instruction)
{
    auto instr = std::static_pointer_cast<Thumb::LoadStoreImmediateInstruction>(instruction);

    GeneralPurposeRegister& Rd = _cpu->GetRegister(instr->GetDestinationRegister());
    GeneralPurposeRegister& Rn = _cpu->GetRegister(instr->GetBaseAddressRegister());

    switch (instr->GetOpcode())
    {
        case Thumb::ThumbOpcodes::LDRB_1:
            Rd = _cpu->GetMemory()->ReadUInt8(Rn + instr->GetImmediate());
            break;
        case Thumb::ThumbOpcodes::LDR_1:
            Rd = _cpu->GetMemory()->ReadUInt32(Rn + instr->GetImmediate());
            break;
        case Thumb::ThumbOpcodes::STRB_1:
            _cpu->GetMemory()->WriteUInt8(Rn + instr->GetImmediate(), Rd);
            break;
        case Thumb::ThumbOpcodes::STR_1:
            _cpu->GetMemory()->WriteUInt8(Rn + instr->GetImmediate(), Rd);
            break;
        case Thumb::ThumbOpcodes::LDRH_1:
            Rd = _cpu->GetMemory()->ReadUInt32(Rn + instr->GetImmediate());
            break;
        case Thumb::ThumbOpcodes::STRH_1:
            _cpu->GetMemory()->WriteUInt32(Rn + instr->GetImmediate(), Rd);
            break;
        default:
            break;
    }
}

// Sections: A7.1.31, A7.1.60
void Interpreter::HandleThumbLoadStoreStackInstruction(std::shared_ptr<ThumbInstruction> instruction)
{
    auto instr = std::static_pointer_cast<Thumb::LoadStoreStackInstruction>(instruction);

    GeneralPurposeRegister& Rd = _cpu->GetRegister(instr->GetDestinationRegister());

    if (instr->IsLoadOperation())
        Rd = _cpu->GetMemory()->ReadUInt32(_cpu->GetRegister(SP) + instr->GetRelativeOffset());
    else
        _cpu->GetMemory()->WriteUInt32(_cpu->GetRegister(SP) + instr->GetRelativeOffset(), Rd);
}

// Section: A7.1.27, A7.1.57
void Interpreter::HandleThumbLoadStoreMultipleInstruction(std::shared_ptr<ThumbInstruction> instruction)
{
    auto instr = std::static_pointer_cast<Thumb::LoadStoreMultipleInstruction>(instruction);

    if (instr->GetOpcode() == Thumb::ThumbOpcodes::LDMIA)
    {
        uint32_t address = _cpu->GetRegister(instr->GetRegister());
        uint32_t registersMask = instr->GetRegisterList();

        bool writeBack = true;

        if (registersMask == 0) // UNPREDICTABLE
        {
            // Vodoo.
        }

        for (uint8_t i = 0; i < 8; ++i)
        {
            if ((registersMask & (1 << i)) == 1)
            {
                _cpu->GetRegister(i) = _cpu->GetMemory()->ReadUInt32(address);
                address += 4;
            }

            if (i == instr->GetRegister())
                writeBack = false;
        }
        // If the base register <Rn> is specified in <registers>,
        // the final value of <Rn> is the loaded value (not the written-back value).

        if (writeBack)
            _cpu->GetRegister(instr->GetRegister()) = address;
    }
    else // if (instr->GetOpcode() == Thumb::ThumbOpcodes::STMIA)
    {
        uint32_t address = _cpu->GetRegister(instr->GetRegister());
        uint32_t registersMask = instr->GetRegisterList();

        if (registersMask == 0) // UNPREDICTABLE
        {
            // Vodoo.
        }

        for (uint8_t i = 0; i < 8; ++i)
        {
            if ((registersMask & (1 << i)) == 1)
            {
                _cpu->GetMemory()->WriteUInt32(address, _cpu->GetRegister(i));
                address += 4;
            }
        }
        // If <Rn> is specified in <registers>:
        //  • If <Rn> is the lowest-numbered register specified in <registers>,
        //    the original value of <Rn> is stored.
        //  • Otherwise, the stored value of <Rn> is UNPREDICTABLE.
    }
}

void Interpreter::HandleThumbBranchLinkInstruction(std::shared_ptr<ThumbInstruction> instruction)
{
    auto branch = std::static_pointer_cast<Thumb::LongBranchLinkInstruction>(instruction);
    
    // If this is just the high part of the instruction, update the LR
    if (!branch->IsTriggeringSubroutineCall())
        _cpu->GetRegister(LR) = _cpu->GetRegister(PC) + 2 + (branch->GetOffset() << 12);
    else
    {
        uint32_t oldPC = _cpu->GetRegister(PC);
        _cpu->GetRegister(PC) = _cpu->GetRegister(LR) + (branch->GetOffset() << 1);
        _cpu->GetRegister(LR) = oldPC | 1;
    }
}

void Interpreter::HandleThumbBranchInstruction(std::shared_ptr<ThumbInstruction> instruction)
{
    auto branch = std::static_pointer_cast<Thumb::BranchInstruction>(instruction);

    if (branch->IsConditionalBranch() && !_cpu->ConditionPasses(InstructionCondition(branch->GetCondition())))
        return;

    _cpu->GetRegister(PC) += branch->GetBranchOffset() + 2;
}