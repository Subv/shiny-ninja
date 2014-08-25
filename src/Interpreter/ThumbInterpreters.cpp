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


void Interpreter::HandleThumbStackOperationInstruction(std::shared_ptr<ThumbInstruction> instruction)
{
    auto stackOperation = std::static_pointer_cast<Thumb::StackOperation>(instruction);

    std::bitset<9> registersSet = stackOperation->GetRegisterMask();
    if (stackOperation->IsPopOperand())
    {
        // MemoryAccess(B-bit, E-bit)
        uint32_t startAddress = _cpu->GetRegister(SP);
        uint32_t endAddress = startAddress + 4 * registersSet.count();
        uint32_t address = startAddress;

        for (uint8_t i = 0; i < 8; ++i)
        {
            if (!registersSet.test(i))
                continue;
            _cpu->GetRegister(i) = _cpu->GetMemory()->ReadUInt32(address);
            address += 4;
        }

        if (registersSet.test(8))
        {
            uint32_t value = _cpu->GetMemory()->ReadUInt32(address);
            _cpu->GetRegister(PC) = value & 0xFFFFFFFE;
            // if (arch v5 or above) // Halp, subv, i'm confused as to what version we use
            //     CPSR.T = value[0];
            address += 4;
        }

        Utilities::Assert(endAddress == address, "");
        _cpu->GetRegister(SP) = endAddress;
    }
    else
    {
        uint32_t startAddress = _cpu->GetRegister(SP) - 4 * registersSet.count();
        uint32_t endAddress = _cpu->GetRegister(SP) - 4U;
        uint32_t address = startAddress;
        for (uint8_t i = 0; i < 8; ++i)
        {
            if (!registersSet.test(i))
                continue;
             _cpu->GetMemory()->WriteUInt32(address, _cpu->GetRegister(i));
            address += 4;
        }

        if (registersSet.test(8))
        {
            _cpu->GetMemory()->WriteUInt32(address, _cpu->GetRegister(LR));
            address += 4;
        }

        Utilities::Assert(endAddress == address - 4, "");
        _cpu->GetRegister(SP) = startAddress;
        // Same as POP
        // if (CP15_reg1_Ubit == 1) /* ARMv6 */
        //   if Shared(address then /* from ARMv6 */
        //     physical_address = TLB(address
        //     ClearExclusiveByAddress(physical_address, size)
    }
}

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
            if (Imm == 0)
                Rd = 0;
            else
                Rd = Rm << Imm;
            break;
        }
        case Thumb::ThumbOpcodes::ASR_1:
        {
            _cpu->GetCurrentStatusFlags().C = Rm[Imm == 0 ? 31 : (Imm - 1)];
            if (Imm == 0)
                Rd = (Rm[31] ? 0 : 0xFFFFFFFF);
            else
                Rd = int32_t(Rm) >> 5;
            break;
        }
    }
    _cpu->GetCurrentStatusFlags().N = Rd[31];
    _cpu->GetCurrentStatusFlags().Z = Rd == 0U;
}

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
            _cpu->GetCurrentStatusFlags().V = MathHelper::OverflowFrom<true>(Rn, Rm);
            _cpu->GetCurrentStatusFlags().C = MathHelper::CarryFrom(Rn, Rm);
            break;
        case Thumb::ThumbOpcodes::SUB_1:
        case Thumb::ThumbOpcodes::SUB_3:
            Rd = Rn - Rm;
            _cpu->GetCurrentStatusFlags().C = !MathHelper::BorrowFrom(Rn, Rm);
            _cpu->GetCurrentStatusFlags().V = MathHelper::OverflowFrom<false>(Rn, Rm);
            break;

    }
    _cpu->GetCurrentStatusFlags().N = Rd[31];
    _cpu->GetCurrentStatusFlags().Z = Rd == 0U;
}

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
            _cpu->GetCurrentStatusFlags().Z = Rd;
            break;
        case Thumb::ThumbOpcodes::CMP_1: // Rd is actually Rn here
        {
            int32_t aluOut = Rd - Imm;
            _cpu->GetCurrentStatusFlags().N = aluOut >> 31;
            _cpu->GetCurrentStatusFlags().Z = aluOut != 0;
            _cpu->GetCurrentStatusFlags().C = !MathHelper::BorrowFrom(Rd, Imm);
            _cpu->GetCurrentStatusFlags().V = MathHelper::OverflowFrom<false>(Rd, Imm);
            break;
        }
        case Thumb::ThumbOpcodes::ADD_2:
            _cpu->GetCurrentStatusFlags().V = MathHelper::OverflowFrom<true>(Rd, Imm);
            Rd += Imm;
            _cpu->GetCurrentStatusFlags().N = Rd[31];
            _cpu->GetCurrentStatusFlags().Z = Rd;
            _cpu->GetCurrentStatusFlags().C = MathHelper::CarryFrom(Rd, Imm);
            break;
        case Thumb::ThumbOpcodes::SUB_2:
            _cpu->GetCurrentStatusFlags().V = MathHelper::OverflowFrom<false>(Rd, Imm);
            Rd -= Imm;
            _cpu->GetCurrentStatusFlags().N = Rd[31];
            _cpu->GetCurrentStatusFlags().Z = Rd;
            _cpu->GetCurrentStatusFlags().C = !MathHelper::BorrowFrom(Rd, Imm);
            break;

    }
}

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
            _cpu->GetCurrentStatusFlags().Z = Rd == 0U;
            break;
        case Thumb::ThumbOpcodes::EOR:
            Rd ^= Rm;
            _cpu->GetCurrentStatusFlags().N = Rd[31];
            _cpu->GetCurrentStatusFlags().Z = Rd == 0U;
            break;
        case Thumb::ThumbOpcodes::LSL_2: // Rm = Rs here
        case Thumb::ThumbOpcodes::LSR_2: // Rm = Rs here
        case Thumb::ThumbOpcodes::ASR_2: // Rm = Rs here
        {
            bool isLeftShift = opcode == Thumb::ThumbOpcodes::LSL_2;
            bool isArithmetic = opcode == Thumb::ThumbOpcodes::ASR_2;
            if (Rm < 32U)
            {
                _cpu->GetCurrentStatusFlags().C = Rd[isLeftShift ? (32U - Rm) : (Rm - 1U)];
                Rd = isLeftShift ? (Rd << Rm) : (isArithmetic ? (Rd >> Rm) : (Rd >> Rm));
            }
            else if (Rm == 0U)
            {
                _cpu->GetCurrentStatusFlags().C = (Rm == 32 || isArithmetic) ? Rd[31] : 0;
                Rd = (isArithmetic ? (Rd[31] == 0 ? 0 : 0xFFFFFFFF) : 0);
            }
            _cpu->GetCurrentStatusFlags().N = Rd[31];
            _cpu->GetCurrentStatusFlags().Z = Rd == 0U;
            break;
        }
        // In ADC and SBC, determine C and V before doing the maths.
        case Thumb::ThumbOpcodes::ADC:
        {
            uint8_t oldCarry = _cpu->GetCurrentStatusFlags().C;
            _cpu->GetCurrentStatusFlags().C = oldCarry ? (Rd > Rm) : (Rd >= Rm);
            _cpu->GetCurrentStatusFlags().V = MathHelper::OverflowFrom<true>(Rd, Rm + _cpu->GetCurrentStatusFlags().C);
            Rd += Rm + oldCarry;
            _cpu->GetCurrentStatusFlags().N = Rd[31];
            _cpu->GetCurrentStatusFlags().Z = Rd == 0;
            break;
        }
        case Thumb::ThumbOpcodes::SBC:
        {
            uint8_t oldCarry = _cpu->GetCurrentStatusFlags().C;
            _cpu->GetCurrentStatusFlags().C = !MathHelper::BorrowFrom(Rd, Rm + !oldCarry);
            Rd -= Rm + !oldCarry;
            _cpu->GetCurrentStatusFlags().N = Rd[31];
            _cpu->GetCurrentStatusFlags().Z = Rd == 0;
            _cpu->GetCurrentStatusFlags().V = MathHelper::OverflowFrom<true>(Rd, Rm + oldCarry);
            break;
        }
        case Thumb::ThumbOpcodes::ROR: // Rm = Rs here
            if (Rm != 0)
            {
                if (Rm.GetBits(0, 5) == 0)
                    _cpu->GetCurrentStatusFlags().C = Rd[31];
                else // if (Rm.GetBits(0, 5) > 0) { Rs[4:0] }
                {
                    // ROR is stored in the LSB.
                    _cpu->GetCurrentStatusFlags().C = Rd[Rm.GetBits(0, 4) - 1];
                    //! FIXME: Does not compile
                    // Rd = MathHelper::RotateRight(Rd, Rm.GetBits(0, 4));
                }
            }
            _cpu->GetCurrentStatusFlags().N = Rd[31];
            _cpu->GetCurrentStatusFlags().Z = Rd == 0U;
            break;
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
            _cpu->GetCurrentStatusFlags().C = !MathHelper::BorrowFrom(0, Rm);
            _cpu->GetCurrentStatusFlags().Z = MathHelper::OverflowFrom<false>(0, Rm);
            break;
        }
        case Thumb::ThumbOpcodes::CMN: // Rd is actually Rn here
        case Thumb::ThumbOpcodes::CMP_2: // Rd is actually Rn here
        {
            int32_t aluOut = (opcode == Thumb::ThumbOpcodes::CMP_2) ? (Rd - Rm) : (Rd + Rm);
            _cpu->GetCurrentStatusFlags().N = aluOut & (1 << 31);
            _cpu->GetCurrentStatusFlags().Z = aluOut != 0;
            if (opcode == Thumb::ThumbOpcodes::CMP_2)
            {
                _cpu->GetCurrentStatusFlags().C = !MathHelper::BorrowFrom(Rd, Rm);
                _cpu->GetCurrentStatusFlags().V = MathHelper::OverflowFrom<false>(Rd, Rm);
            }
            else
            {
                _cpu->GetCurrentStatusFlags().C = MathHelper::CarryFrom(Rd, Rm);
                _cpu->GetCurrentStatusFlags().V = MathHelper::OverflowFrom<true>(Rd, Rm);
            }
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

void Interpreter::HandleThumbSpecialDataProcessingInstruction(std::shared_ptr<ThumbInstruction> instruction)
{
    auto realInstr = std::static_pointer_cast<Thumb::SpecialDataProcessingInstruction>(instruction);
    switch (realInstr->GetOpcode())
    {
        case Thumb::ThumbOpcodes::ADD_4:
        {
            GeneralPurposeRegister& Rd = _cpu->GetRegister(realInstr->GetDestinationRegister());
            GeneralPurposeRegister& Rm = _cpu->GetRegister(realInstr->GetFirstDataRegister());
            Rd += Rm;
            break;
        }
        case Thumb::ThumbOpcodes::CMP_3:
        {
            GeneralPurposeRegister& Rd = _cpu->GetRegister(realInstr->GetDestinationRegister());
            GeneralPurposeRegister& Rm = _cpu->GetRegister(realInstr->GetFirstDataRegister());
            if (realInstr->HiOperandFlagDestination() == 0 && realInstr->HiOperandFlagSource())
            {
                // UNPREDICTABLE
                return;
            }
            uint32_t aluOut = Rd - Rm;
            _cpu->GetCurrentStatusFlags().N = aluOut & (1 << 31);
            _cpu->GetCurrentStatusFlags().Z = aluOut == 0;
            _cpu->GetCurrentStatusFlags().C = !MathHelper::BorrowFrom(Rd, Rm);
            _cpu->GetCurrentStatusFlags().V = MathHelper::OverflowFrom<false>(Rd, Rm);
            break;
        }
        case Thumb::ThumbOpcodes::MOV_3:
        {
            GeneralPurposeRegister& Rd = _cpu->GetRegister(realInstr->GetDestinationRegister());
            GeneralPurposeRegister& Rm = _cpu->GetRegister(realInstr->GetFirstDataRegister());
            Rd = Rm;
            break;
        }
        default:
            break;
    }
}

//! TODO FIXME
void Interpreter::HandleThumbBranchExchangeInstruction(std::shared_ptr<ThumbInstruction> instruction)
{
    auto realInstr = std::static_pointer_cast<Thumb::BranchExchangeInstruction>(instruction);

    switch (realInstr->GetOpcode())
    {
        case Thumb::ThumbOpcodes::BX:
        {
            GeneralPurposeRegister& Rm = _cpu->GetRegister(realInstr->GetTargetAddressRegister());
            _cpu->GetCurrentStatusFlags().T = Rm[0];
            _cpu->GetRegister(PC) = Rm.GetBits(1, 31) << 1;
            break;
        }
        case Thumb::ThumbOpcodes::BLX_2: // Currently not handled, me uber confused
        {
            if (realInstr->GetTargetAddressRegister() == 15)
            {
                // UNPREDICTABLE
            }

            uint32_t Rm = _cpu->GetRegister(realInstr->GetTargetAddressRegister());
            if (Rm & 2)
            {
                // UNPREDICTABLE
            }
            // LR = (address of this instruction after the BLX) | 1
            // _cpu->GetRegister(LR) =
            _cpu->GetCurrentStatusFlags().T = Rm & 1;
            _cpu->GetRegister(PC) = Rm & 0xFFFFFFFE;
            break;
        }
    }
}

void Interpreter::HandleThumbLiteralPoolLoadInstruction(std::shared_ptr<ThumbInstruction> instruction)
{
    auto instr = std::static_pointer_cast<Thumb::LoadFromLiteralStoreInstruction>(instruction);
    uint32_t address = _cpu->GetRegister(PC) & 0xFFFFFFFC + instr->GetImmediate();
    _cpu->GetRegister(instr->GetDestinationRegister()) = _cpu->GetMemory()->ReadUInt32(address);
}

void Interpreter::HandleThumbLoadStoreRegisterOffsetInstruction(std::shared_ptr<ThumbInstruction> instruction)
{
    auto instr = std::static_pointer_cast<Thumb::LoadStoreRegisterOffsetInstruction>(instruction);

    GeneralPurposeRegister& Rd = _cpu->GetRegister(instr->GetDestinationRegister());
    GeneralPurposeRegister& Rn = _cpu->GetRegister(instr->GetFirstOperand());
    GeneralPurposeRegister& Rm = _cpu->GetRegister(instr->GetSecondOperand());

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
            _cpu->GetMemory()->WriteUInt8(Rn + Rm, Rd);
            break;
        case Thumb::ThumbOpcodes::STRH_2:
            _cpu->GetMemory()->WriteUInt16(Rn + Rm, Rd);
            break;
        case Thumb::ThumbOpcodes::LDRH_2:
            Rd = _cpu->GetMemory()->ReadUInt16(Rn + Rm);
            break;
        case Thumb::ThumbOpcodes::LDRB_2:
            Rd = _cpu->GetMemory()->ReadUInt8(Rn + Rm);
            break;
    }
}

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
            //! TODO: Remove * 4 when GetImmediate() is fixed (doesn't do the math yet)
            //! Since not all opcodes under this format need a multiplication...
            //! Warpten.
            Rd = _cpu->GetMemory()->ReadUInt32(Rn + instr->GetImmediate() << 2);
            break;
        case Thumb::ThumbOpcodes::STRB_1:
            _cpu->GetMemory()->WriteUInt8(Rn + instr->GetImmediate(), Rd);
            break;
        case Thumb::ThumbOpcodes::STR_1:
            //! TODO: Remove * 4 when GetImmediate() is fixed (doesn't do the math yet)
            //! Since not all opcodes under this format need a multiplication...
            //! Warpten.
            _cpu->GetMemory()->WriteUInt8(Rn + instr->GetImmediate() << 2, Rd);
            break;
        case Thumb::ThumbOpcodes::LDRH_1:
            Rd = _cpu->GetMemory()->ReadUInt32(Rn + instr->GetImmediate() << 1);
            break;
        case Thumb::ThumbOpcodes::STRH_1:
            _cpu->GetMemory()->WriteUInt32(Rn + instr->GetImmediate() << 1, Rd);
            break;
        default:
            break;
    }
}

void Interpreter::HandleThumbLoadStoreStackInstruction(std::shared_ptr<ThumbInstruction> instruction)
{
    auto instr = std::static_pointer_cast<Thumb::LoadStoreStackInstruction>(instruction);

    GeneralPurposeRegister& Rd = _cpu->GetRegister(instr->GetDestinationRegister());

    switch (instr->GetOpcode())
    {
        case Thumb::ThumbOpcodes::ADD_6:
            Rd = _cpu->GetRegister(SP) + instr->GetRelativeOffset() << 2;
            break;
        case Thumb::ThumbOpcodes::ADD_5:
            Rd = _cpu->GetRegister(PC) & 0xFFFFFFFC + instr->GetRelativeOffset() << 2;
            break;
        default:
            break;
    }
}

void Interpreter::HandleThumbLoadStoreMultipleInstruction(std::shared_ptr<ThumbInstruction> instruction)
{
    auto instr = std::static_pointer_cast<Thumb::LoadStoreMultipleInstruction>(instruction);

    if (instr->GetOpcode() == Thumb::ThumbOpcodes::LDMIA)
    {
        uint32_t address = _cpu->GetRegister(instr->GetRegister());
        uint32_t registersMask = instr->GetRegisterList();
        for (uint8_t i = 0; i < 8; ++i)
        {
            if ((registersMask & (1 << i)) == 1)
            {
                _cpu->GetRegister(i) = _cpu->GetMemory()->ReadUInt32(address);
                address += 4;
            }
        }
        // If the base register <Rn> is specified in <registers>,
        // the final value of <Rn> is the loaded value (not the written-back value).
    }
    else // if (instr->GetOpcode() == Thumb::ThumbOpcodes::STMIA)
    {
        uint32_t address = _cpu->GetRegister(instr->GetRegister());
        uint32_t registersMask = instr->GetRegisterList();
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
