#include "Interpreter.hpp"

#include "CPU/CPU.hpp"

#include "Common/Instructions/ARMInstruction.hpp"
#include "Common/Instructions/ThumbInstruction.hpp"

#include "Common/Instructions/ARM/BranchInstructions.hpp"
#include "Common/Instructions/ARM/DataProcessingInstructions.hpp"
#include "Common/Instructions/ARM/LoadStoreInstructions.hpp"
#include "Common/Instructions/ARM/PSRTransferInstructions.hpp"
#include "Common/Instructions/ARM/MultiplyAccumulateInstructions.hpp"

#include "Common/Instructions/Thumb/MiscInstructions.hpp"
#include "Common/Instructions/Thumb/DataProcessingInstructions.hpp"

#include "Common/MathHelper.hpp"
#include "Common/Utilities.hpp"

#include <bitset>

Interpreter::Interpreter(CPU* arm)  : _cpu(arm)
{
    InitializeHandlers();
}

void Interpreter::RunInstruction(std::shared_ptr<Instruction> instruction)
{
    if (instruction->GetInstructionSet() == InstructionSet::ARM)
        HandleARM(std::static_pointer_cast<ARMInstruction>(instruction));
    else if (instruction->GetInstructionSet() == InstructionSet::Thumb)
        HandleThumb(std::static_pointer_cast<ThumbInstruction>(instruction));
    else
        Utilities::Assert(false, "Invalid instruction set");
}

void Interpreter::HandleARM(std::shared_ptr<ARMInstruction> instruction)
{
    auto handler = _armHandlers.find(ARM::ARMOpcodes(instruction->GetOpcode()));

    if (handler != _armHandlers.end())
        handler->second(instruction);
}

void Interpreter::HandleThumb(std::shared_ptr<ThumbInstruction> instruction)
{
    auto handler = _thumbHandlers.find(Thumb::ThumbOpcodes(instruction->GetOpcode()));

    if (handler != _thumbHandlers.end())
        handler->second(instruction);
}

void Interpreter::InitializeHandlers()
{
    InitializeArm();
    InitializeThumb();
}

void Interpreter::InitializeArm()
{
    // Branch Instructions
    _armHandlers[ARM::ARMOpcodes::B] = std::bind(&Interpreter::HandleARMBranchInstruction, this, std::placeholders::_1);
    _armHandlers[ARM::ARMOpcodes::BL] = std::bind(&Interpreter::HandleARMBranchInstruction, this, std::placeholders::_1);
    _armHandlers[ARM::ARMOpcodes::BX] = std::bind(&Interpreter::HandleARMBranchInstruction, this, std::placeholders::_1);
    _armHandlers[ARM::ARMOpcodes::BLX] = std::bind(&Interpreter::HandleARMBranchInstruction, this, std::placeholders::_1);

    // Data Processing Instructions
    _armHandlers[ARM::ARMOpcodes::AND] = std::bind(&Interpreter::HandleARMDataProcessingInstruction, this, std::placeholders::_1);
    _armHandlers[ARM::ARMOpcodes::EOR] = std::bind(&Interpreter::HandleARMDataProcessingInstruction, this, std::placeholders::_1);
    _armHandlers[ARM::ARMOpcodes::SUB] = std::bind(&Interpreter::HandleARMDataProcessingInstruction, this, std::placeholders::_1);
    _armHandlers[ARM::ARMOpcodes::RSB] = std::bind(&Interpreter::HandleARMDataProcessingInstruction, this, std::placeholders::_1);
    _armHandlers[ARM::ARMOpcodes::ADD] = std::bind(&Interpreter::HandleARMDataProcessingInstruction, this, std::placeholders::_1);
    _armHandlers[ARM::ARMOpcodes::ADC] = std::bind(&Interpreter::HandleARMDataProcessingInstruction, this, std::placeholders::_1);
    _armHandlers[ARM::ARMOpcodes::SBC] = std::bind(&Interpreter::HandleARMDataProcessingInstruction, this, std::placeholders::_1);
    _armHandlers[ARM::ARMOpcodes::RSC] = std::bind(&Interpreter::HandleARMDataProcessingInstruction, this, std::placeholders::_1);
    _armHandlers[ARM::ARMOpcodes::TST] = std::bind(&Interpreter::HandleARMDataProcessingInstruction, this, std::placeholders::_1);
    _armHandlers[ARM::ARMOpcodes::TEQ] = std::bind(&Interpreter::HandleARMDataProcessingInstruction, this, std::placeholders::_1);
    _armHandlers[ARM::ARMOpcodes::CMP] = std::bind(&Interpreter::HandleARMDataProcessingInstruction, this, std::placeholders::_1);
    _armHandlers[ARM::ARMOpcodes::CMN] = std::bind(&Interpreter::HandleARMDataProcessingInstruction, this, std::placeholders::_1);
    _armHandlers[ARM::ARMOpcodes::ORR] = std::bind(&Interpreter::HandleARMDataProcessingInstruction, this, std::placeholders::_1);
    _armHandlers[ARM::ARMOpcodes::MOV] = std::bind(&Interpreter::HandleARMDataProcessingInstruction, this, std::placeholders::_1);
    _armHandlers[ARM::ARMOpcodes::BIC] = std::bind(&Interpreter::HandleARMDataProcessingInstruction, this, std::placeholders::_1);
    _armHandlers[ARM::ARMOpcodes::MVN] = std::bind(&Interpreter::HandleARMDataProcessingInstruction, this, std::placeholders::_1);

    // Load / Store instructions
    _armHandlers[ARM::ARMOpcodes::LDR] = std::bind(&Interpreter::HandleARMLoadStoreInstruction, this, std::placeholders::_1);
    _armHandlers[ARM::ARMOpcodes::LDRB] = std::bind(&Interpreter::HandleARMLoadStoreInstruction, this, std::placeholders::_1);
    _armHandlers[ARM::ARMOpcodes::LDRBT] = std::bind(&Interpreter::HandleARMLoadStoreInstruction, this, std::placeholders::_1);
    _armHandlers[ARM::ARMOpcodes::STR] = std::bind(&Interpreter::HandleARMLoadStoreInstruction, this, std::placeholders::_1);
    _armHandlers[ARM::ARMOpcodes::STRB] = std::bind(&Interpreter::HandleARMLoadStoreInstruction, this, std::placeholders::_1);
    _armHandlers[ARM::ARMOpcodes::STRBT] = std::bind(&Interpreter::HandleARMLoadStoreInstruction, this, std::placeholders::_1);

    // Miscellaneous Load / Store instructions
    _armHandlers[ARM::ARMOpcodes::STRH] = std::bind(&Interpreter::HandleARMMiscellaneousLoadStoreInstruction, this, std::placeholders::_1);
    _armHandlers[ARM::ARMOpcodes::LDRH] = std::bind(&Interpreter::HandleARMMiscellaneousLoadStoreInstruction, this, std::placeholders::_1);
    _armHandlers[ARM::ARMOpcodes::LDRSH] = std::bind(&Interpreter::HandleARMMiscellaneousLoadStoreInstruction, this, std::placeholders::_1);
    _armHandlers[ARM::ARMOpcodes::LDRSB] = std::bind(&Interpreter::HandleARMMiscellaneousLoadStoreInstruction, this, std::placeholders::_1);

    // Not Yet Implemented
    _armHandlers[ARM::ARMOpcodes::LDM] = std::bind(&Interpreter::HandleARMLoadStoreMultipleInstruction, this, std::placeholders::_1);
    _armHandlers[ARM::ARMOpcodes::STM] = std::bind(&Interpreter::HandleARMLoadStoreMultipleInstruction, this, std::placeholders::_1);

    // PSR Operations
    _armHandlers[ARM::ARMOpcodes::MRS] = std::bind(&Interpreter::HandleARMPSROperationInstruction, this, std::placeholders::_1);
    _armHandlers[ARM::ARMOpcodes::MSR] = std::bind(&Interpreter::HandleARMPSROperationInstruction, this, std::placeholders::_1);

    // Multiply and Multiply Accumulate
    _armHandlers[ARM::ARMOpcodes::MUL] = std::bind(&Interpreter::HandleARMMultiplyInstruction, this, std::placeholders::_1);
    _armHandlers[ARM::ARMOpcodes::MLA] = std::bind(&Interpreter::HandleARMMultiplyInstruction, this, std::placeholders::_1);
    _armHandlers[ARM::ARMOpcodes::SMLAL] = std::bind(&Interpreter::HandleARMMultiplyInstruction, this, std::placeholders::_1);
    _armHandlers[ARM::ARMOpcodes::SMULL] = std::bind(&Interpreter::HandleARMMultiplyInstruction, this, std::placeholders::_1);
    _armHandlers[ARM::ARMOpcodes::UMLAL] = std::bind(&Interpreter::HandleARMMultiplyInstruction, this, std::placeholders::_1);
    _armHandlers[ARM::ARMOpcodes::UMULL] = std::bind(&Interpreter::HandleARMMultiplyInstruction, this, std::placeholders::_1);
}

void Interpreter::InitializeThumb()
{
    // Stack operations
    _thumbHandlers[Thumb::ThumbOpcodes::PUSH] = std::bind(&Interpreter::HandleThumbStackOperationInstruction, this, std::placeholders::_1);
    _thumbHandlers[Thumb::ThumbOpcodes::POP] = std::bind(&Interpreter::HandleThumbStackOperationInstruction, this, std::placeholders::_1);

    // Immediate Shift operations
    _thumbHandlers[Thumb::ThumbOpcodes::LSL_1] = std::bind(&Interpreter::HandleThumbImmediateShiftInstruction, this, std::placeholders::_1);
    _thumbHandlers[Thumb::ThumbOpcodes::LSR_1] = std::bind(&Interpreter::HandleThumbImmediateShiftInstruction, this, std::placeholders::_1);
    _thumbHandlers[Thumb::ThumbOpcodes::ASR_1] = std::bind(&Interpreter::HandleThumbImmediateShiftInstruction, this, std::placeholders::_1);

    // Add/Substract Register/Immediate operations
    _thumbHandlers[Thumb::ThumbOpcodes::ADD_1] = std::bind(&Interpreter::HandleThumbAddSubImmRegInstruction, this, std::placeholders::_1);
    _thumbHandlers[Thumb::ThumbOpcodes::ADD_3] = std::bind(&Interpreter::HandleThumbAddSubImmRegInstruction, this, std::placeholders::_1);
    _thumbHandlers[Thumb::ThumbOpcodes::SUB_1] = std::bind(&Interpreter::HandleThumbAddSubImmRegInstruction, this, std::placeholders::_1);
    _thumbHandlers[Thumb::ThumbOpcodes::SUB_3] = std::bind(&Interpreter::HandleThumbAddSubImmRegInstruction, this, std::placeholders::_1);
}

void Interpreter::HandleARMBranchInstruction(std::shared_ptr<ARMInstruction> instruction)
{
    if (instruction->GetOpcode() == ARM::ARMOpcodes::BLX || instruction->GetOpcode() == ARM::ARMOpcodes::BX)
    {
        // Save the return address, only the BLX instruction does this
        if (instruction->GetOpcode() == ARM::ARMOpcodes::BLX)
            _cpu->GetRegister(LR) = _cpu->GetRegister(PC);

        if (instruction->IsImmediate())
        {
            auto branch = std::static_pointer_cast<ARM::BranchLinkExchangeImmediateInstruction>(instruction);
            // The second bit of the signed offset is set to the value specified in the opcode (GetSecondBit())
            // The 4 is due to the CPU pipeline, the prefetch has already happened so PC should be at <CurrentInstruction> + 8, however we are only at <CurrentInstruction> + 4
            _cpu->GetRegister(PC) += 4 + branch->GetSignedOffset() + (branch->GetSecondBit() << 1);

            // This version of BLX always switches to Thumb mode
            _cpu->SetInstructionSet(InstructionSet::Thumb);
        }
        else
        {
            auto branch = std::static_pointer_cast<ARM::BranchLinkExchangeRegisterInstruction>(instruction);

            // The value of the T bit will be that of the first bit of the value stored in the specified register
            uint32_t reg = _cpu->GetRegister(branch->GetRegister());
            bool thumb = MathHelper::CheckBit(reg, 0);

            // The branch location is stored in a register, with the first bit set to 0
            _cpu->GetRegister(PC) = reg & 0xFFFFFFFE;

            _cpu->SetInstructionSet(thumb ? InstructionSet::Thumb : InstructionSet::ARM);
        }

        return;
    }

    auto branch = std::static_pointer_cast<ARM::BranchInstruction>(instruction);

    if (branch->Link())
        _cpu->GetRegister(LR) = _cpu->GetRegister(PC);

    // We have to add 4 because the CPU pipeline prefetches the next opcode.
    _cpu->GetRegister(PC) += branch->GetSignedOffset() + 4;
}

void Interpreter::HandleARMDataProcessingInstruction(std::shared_ptr<ARMInstruction> instruction)
{
    auto dataproc = std::static_pointer_cast<ARM::DataProcessingInstruction>(instruction);

    int64_t firstOperand = _cpu->GetRegister(dataproc->GetFirstOperand());
    
    // Account for CPU prefetch, the code expects the PC to be at <CurrentInstruction> + 8, but we're currently at <CurrentInstruction> + 4
    if (dataproc->GetFirstOperand() == PC)
        firstOperand += 4;

    // Use int64_t so that we can check for overflow later
    int64_t secondOperand = 0;
    int64_t result = 0;

    // Default to the previous Carry value
    bool carryOut = _cpu->GetCurrentStatusFlags().C;

    if (dataproc->IsImmediate())
        secondOperand = dataproc->GetShiftedSecondOperandImmediate();
    else
    {
        int32_t registerValue = _cpu->GetRegister(dataproc->GetSecondOperand());
        
        if (dataproc->GetSecondOperand() == PC)
            registerValue += 4;

        // If the instruction must shift the second operand by a register value, then only the lower 8 bits of that register are used.
        uint8_t shiftValue = dataproc->ShiftByRegister() ? ((_cpu->GetRegister(dataproc->GetShiftRegisterOrImmediate()) + (dataproc->GetShiftRegisterOrImmediate() == PC ? 4 : 0)) & 0xFF) : dataproc->GetShiftRegisterOrImmediate();

        switch (dataproc->GetShiftType())
        {
            case ARM::ShiftType::LSL:
                secondOperand = registerValue << shiftValue;
                if (shiftValue)
                    carryOut = MathHelper::CheckBit(registerValue, 32 - shiftValue);
                break;
            case ARM::ShiftType::ASR:
                // Handle a special case for the ASR shifter as defined in the ARM Reference Manual
                if ((!dataproc->ShiftByRegister() && shiftValue == 0) || (dataproc->ShiftByRegister() && shiftValue >= 32))
                {
                    carryOut = MathHelper::CheckBit(registerValue, 31);
                    if (carryOut)
                        secondOperand = 0xFFFFFFFF;
                    else
                        secondOperand = 0;
                    break;
                }
                // No break
            case ARM::ShiftType::LSR:
                if (shiftValue)
                    carryOut = MathHelper::CheckBit(registerValue, shiftValue - 1);
                else if (!dataproc->ShiftByRegister())
                    carryOut = MathHelper::CheckBit(registerValue, 31);

                secondOperand = registerValue >> shiftValue;
                break;
            case ARM::ShiftType::ROR:
                if (shiftValue || dataproc->ShiftByRegister())
                {
                    secondOperand = MathHelper::RotateRight(registerValue, shiftValue);
                    if (dataproc->ShiftByRegister())
                    {
                        if (MathHelper::CheckBits(shiftValue, 0, 4, 0))
                            carryOut = MathHelper::CheckBit(registerValue, 31);
                        else
                            carryOut = MathHelper::CheckBit(registerValue, (shiftValue & 0xF) - 1);
                    }
                    else
                        carryOut = MathHelper::CheckBit(registerValue, shiftValue - 1);
                }
                else if (!shiftValue)
                {
                    // A 0 shiftValue specifies a RRX shift
                    secondOperand = (_cpu->GetCurrentStatusFlags().C << 31) | (registerValue >> 1);
                    carryOut = MathHelper::CheckBit(registerValue, 0);
                }
                break;
        }
    }

    switch (dataproc->GetOpcode())
    {
        case ARM::ARMOpcodes::AND:
        case ARM::ARMOpcodes::TST:
            result = firstOperand & secondOperand;
            break;
        case ARM::ARMOpcodes::EOR:
        case ARM::ARMOpcodes::TEQ:
            result = firstOperand ^ secondOperand;
            break;
        case ARM::ARMOpcodes::SUB:
        case ARM::ARMOpcodes::CMP:
            result = firstOperand - secondOperand;
            break;
        case ARM::ARMOpcodes::RSB: // Reversed Subtract
            result = secondOperand - firstOperand;
            break;
        case ARM::ARMOpcodes::ADD:
        case ARM::ARMOpcodes::CMN:
            result = firstOperand + secondOperand;
            break;
        case ARM::ARMOpcodes::ADC: // Add with carry
            result = firstOperand + secondOperand + _cpu->GetCurrentStatusFlags().C;
            break;
        case ARM::ARMOpcodes::SBC: // Sub with carry
            result = firstOperand - secondOperand + _cpu->GetCurrentStatusFlags().C - 1;
            break;
        case ARM::ARMOpcodes::RSC: // Reversed Sub with Carry
            result = secondOperand - firstOperand + _cpu->GetCurrentStatusFlags().C - 1;
            break;
        case ARM::ARMOpcodes::ORR:
            result = firstOperand | secondOperand;
            break;
        case ARM::ARMOpcodes::MOV:
            result = secondOperand;
            break;
        case ARM::ARMOpcodes::BIC:
            result = firstOperand & ~secondOperand;
            break;
        case ARM::ARMOpcodes::MVN:
            result = ~secondOperand;
            break;
    }

    if (dataproc->SetConditionCodes())
    {
        if (dataproc->GetDestinationRegister() == PC)
        {
            Utilities::Assert(false, "Loading SPSR into CPSR is not yet implemented");
            return;
        }

        _cpu->GetCurrentStatusFlags().N = MathHelper::CheckBit(uint32_t(result), 31);
        _cpu->GetCurrentStatusFlags().Z = result == 0;
        
        if (dataproc->AffectsOverflow())
        {
            // For these opcodes, the carry is set based on the result of the shift operation
            if (dataproc->IsImmediate() && dataproc->GetShiftImmediate() != 0)
                _cpu->GetCurrentStatusFlags().C = MathHelper::CheckBit(dataproc->GetShiftedSecondOperandImmediate(), 31);
            else
                _cpu->GetCurrentStatusFlags().C = carryOut;
        }
        else
        {
            // The carry depends on the value of the result for these opcodes

            // For these opcodes, the Carry flag is actually a NOT(Borrow)
            if (dataproc->GetOpcode() == ARM::ARMOpcodes::CMP || dataproc->GetOpcode() == ARM::ARMOpcodes::SUB || dataproc->GetOpcode() == ARM::ARMOpcodes::SBC || dataproc->GetOpcode() == ARM::ARMOpcodes::RSC || dataproc->GetOpcode() == ARM::ARMOpcodes::RSB)
                _cpu->GetCurrentStatusFlags().C = result >= 0;

            _cpu->GetCurrentStatusFlags().C = MathHelper::CheckBit(result, 32); // Check the 32th bit of the result, it's set only if the operation had a carry
        }

        // Now that the carry is set, we can compute the overflow
        if (dataproc->AffectsOverflow())
            _cpu->GetCurrentStatusFlags().V = _cpu->GetCurrentStatusFlags().C ^ _cpu->GetCurrentStatusFlags().N;
    }

    if (dataproc->HasDestinationRegister())
        _cpu->GetRegister(dataproc->GetDestinationRegister()) = int32_t(result);
}

void Interpreter::HandleARMLoadStoreInstruction(std::shared_ptr<ARMInstruction> instr)
{
    auto instruction = std::static_pointer_cast<ARM::LoadStoreInstruction>(instr);

    uint32_t address = _cpu->GetRegister(instruction->GetBaseRegister());
    
    // Account for CPU prefetch, the code expects the PC to be at <CurrentInstruction> + 8, but we're currently at <CurrentInstruction> + 4
    if (instruction->GetBaseRegister() == PC)
        address += 4;

    uint32_t secondAddressValue = 0;

    if (instruction->IsImmediate())
        secondAddressValue = instruction->GetImmediateOffset();
    else
    {
        int32_t registerValue = _cpu->GetRegister(instruction->GetIndexRegister());
        
        // Account for CPU prefetch, the code expects the PC to be at <CurrentInstruction> + 8, but we're currently at <CurrentInstruction> + 4
        if (instruction->GetIndexRegister() == PC)
            registerValue += 4;

        uint32_t shiftValue = instruction->GetShiftImmediate();

        switch (instruction->GetShiftType())
        {
            case ARM::ShiftType::LSL:
                secondAddressValue = registerValue << shiftValue;
                break;
            case ARM::ShiftType::LSR:
                if (shiftValue == 0)
                    shiftValue = 32;
                secondAddressValue = registerValue >> shiftValue;
                break;
            case ARM::ShiftType::ASR:
                if (shiftValue == 0)
                    secondAddressValue = MathHelper::CheckBit(registerValue, 31) ? 0xFFFFFFFF : 0;
                else
                    secondAddressValue = registerValue >> shiftValue;
                break;
            case ARM::ShiftType::ROR:
                // A shift amount of 0 in ROR specifies the RRX shift type
                if (shiftValue == 0)
                    secondAddressValue = (_cpu->GetCurrentStatusFlags().C << 31) | (registerValue >> 1);
                else
                    secondAddressValue = MathHelper::RotateRight(registerValue, shiftValue);
                break;
            default:
                Utilities::Assert(false, "Unknown ShiftType");
                break;
        }
    }

    if (instruction->IsPreIndexed())
    {
        if (instruction->IsBaseAdded())
            address += secondAddressValue;
        else
            address -= secondAddressValue;

        if (instruction->WriteBack() && _cpu->ConditionPasses(instruction->GetCondition()))
            _cpu->GetRegister(instruction->GetBaseRegister()) = address;
    }
    else
    {
        if (instruction->WriteBack() && _cpu->ConditionPasses(instruction->GetCondition()))
        {
            if (instruction->IsBaseAdded())
                _cpu->GetRegister(instruction->GetBaseRegister()) += secondAddressValue;
            else
                _cpu->GetRegister(instruction->GetBaseRegister()) -= secondAddressValue;
        }
    }
    
    switch (instruction->GetOpcode())
    {
        case ARM::ARMOpcodes::LDR:
        case ARM::ARMOpcodes::LDRB:
        case ARM::ARMOpcodes::LDRBT:
            if (instruction->IsUnsignedByte())
                _cpu->GetRegister(instruction->GetRegister()) = _cpu->GetMemory()->ReadUInt8(address);
            else
                _cpu->GetRegister(instruction->GetRegister()) = _cpu->GetMemory()->ReadUInt32(address);
            break;
        case ARM::ARMOpcodes::STR:
        case ARM::ARMOpcodes::STRB:
        case ARM::ARMOpcodes::STRBT:
        {
            uint32_t writeVal = _cpu->GetRegister(instruction->GetRegister());
            if (instruction->GetRegister() == PC)
                writeVal += 4;

            if (instruction->IsUnsignedByte())
                _cpu->GetMemory()->WriteUInt8(address, writeVal & 0xFF);
            else
                _cpu->GetMemory()->WriteUInt32(address, writeVal);
            break;
        }
        default:
            Utilities::Assert(false, "Load/Store instruction is not yet supported");
            break;
    }
}

void Interpreter::HandleARMMiscellaneousLoadStoreInstruction(std::shared_ptr<ARMInstruction> instr)
{
    auto instruction = std::static_pointer_cast<ARM::MiscellaneousLoadStoreInstruction>(instr);

    uint32_t address = _cpu->GetRegister(instruction->GetBaseRegister());

    // Account for CPU prefetch, the code expects the PC to be at <CurrentInstruction> + 8, but we're currently at <CurrentInstruction> + 4
    if (instruction->GetBaseRegister() == PC)
        address += 4;

    uint32_t secondAddressValue = 0;

    if (instruction->IsImmediate())
        secondAddressValue = instruction->GetImmediateOffset();
    else
    {
        secondAddressValue = _cpu->GetRegister(instruction->GetRegisterOffset());
        if (instruction->GetRegisterOffset() == PC)
            secondAddressValue += 4;
    }

    if (instruction->IsPreIndexed())
    {
        if (instruction->IsBaseAdded())
            address += secondAddressValue;
        else
            address -= secondAddressValue;

        if (instruction->WriteBack() && _cpu->ConditionPasses(instruction->GetCondition()))
            _cpu->GetRegister(instruction->GetBaseRegister()) = address;
    }
    else
    {
        if (_cpu->ConditionPasses(instruction->GetCondition()))
        {
            if (instruction->IsBaseAdded())
                _cpu->GetRegister(instruction->GetBaseRegister()) += secondAddressValue;
            else
                _cpu->GetRegister(instruction->GetBaseRegister()) -= secondAddressValue;
        }
    }

    switch (instruction->GetOpcode())
    {
        case ARM::ARMOpcodes::STRH:
        {
            uint32_t writeVal = _cpu->GetRegister(instruction->GetRegister());
            if (instruction->GetRegister() == PC)
                writeVal += 4;
            _cpu->GetMemory()->WriteUInt16(address, writeVal & 0xFFFF);
            break;
        }
        case ARM::ARMOpcodes::LDRH:
            _cpu->GetRegister(instruction->GetRegister()) = _cpu->GetMemory()->ReadUInt16(address);
            break;
        case ARM::ARMOpcodes::LDRSH:
            _cpu->GetRegister(instruction->GetRegister()) = (int16_t)_cpu->GetMemory()->ReadUInt16(address);
            break;
        case ARM::ARMOpcodes::LDRSB:
            _cpu->GetRegister(instruction->GetRegister()) = (int8_t)_cpu->GetMemory()->ReadUInt8(address);
            break;
        default:
            Utilities::Assert(false, "Load/Store instruction is not yet supported");
            break;
    }
}

void Interpreter::HandleARMPSROperationInstruction(std::shared_ptr<ARMInstruction> instruction)
{
    if (instruction->GetOpcode() == ARM::ARMOpcodes::MRS)
    {
        auto mrs = std::static_pointer_cast<ARM::MovePSRToRegisterInstruction>(instruction);

        if (mrs->GetPSRType() == ARM::PSRType::CPSR)
            _cpu->GetRegister(mrs->GetDestinationRegister()) = _cpu->GetCurrentStatusRegister().Full;
        else
            _cpu->GetRegister(mrs->GetDestinationRegister()) = _cpu->GetSavedStatusRegister().Full;
    }
    else
    {
        uint32_t operand = 0;
        uint8_t fieldMask = 0;
        ARM::PSRType psr;

        if (instruction->IsImmediate())
        {
            auto msr = std::static_pointer_cast<ARM::MoveRegisterToPSRImmediateInstruction>(instruction);
            operand = MathHelper::RotateRight(uint32_t(msr->GetImmediateValue()), msr->GetImmediateShift() << 1);
            fieldMask = msr->GetFieldsMask();
            psr = msr->GetPSRType();
        }
        else
        {
            auto msr = std::static_pointer_cast<ARM::MoveRegisterToPSRRegisterInstruction>(instruction);
            operand = _cpu->GetRegister(msr->GetSourceRegister());
            fieldMask = msr->GetFieldsMask();
            psr = msr->GetPSRType();

            if (msr->GetSourceRegister() == PC)
                operand += 4;
        }

        if (operand & BitMaskConstants::UnallocMask)
        {
            Utilities::Assert(false, "Undefined behavior, MSR attempted to set reserved bits");
            return;
        }

        uint32_t byteMask =
            (MathHelper::CheckBit(fieldMask, 0) ? 0x000000FF : 0) |
            (MathHelper::CheckBit(fieldMask, 1) ? 0x0000FF00 : 0) |
            (MathHelper::CheckBit(fieldMask, 2) ? 0x00FF0000 : 0) |
            (MathHelper::CheckBit(fieldMask, 3) ? 0xFF000000 : 0);

        uint32_t mask = 0;

        if (psr == ARM::PSRType::CPSR)
        {
            if (_cpu->IsInPrivilegedMode())
            {
                if (operand & BitMaskConstants::StateMask)
                {
                    Utilities::Assert(false, "Undefined behavior, MSR attempted to set non-ARM execution state");
                    return;
                }

                mask = byteMask & (BitMaskConstants::UserMask | BitMaskConstants::PrivMask);
            }
            else
                mask = byteMask & BitMaskConstants::UserMask;

            _cpu->GetCurrentStatusRegister().Full = (_cpu->GetCurrentStatusRegister().Full & ~mask) | (operand & mask);
        }
        else
        {
            // Modes without an SPSR will assert in CPU::GetSavedStatusRegister
            mask = byteMask & (BitMaskConstants::UserMask | BitMaskConstants::PrivMask | BitMaskConstants::StateMask);
            _cpu->GetSavedStatusRegister().Full = (_cpu->GetSavedStatusRegister().Full & ~mask) | (operand & mask);
        }
    }
}

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
        uint32_t endAddress = _cpu->GetRegister(SP) - 4;
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
                _cpu->GetCurrentStatusRegister().Flags.C = Rm[32 - Imm];
            break;
        }
        case Thumb::ThumbOpcodes::LSR_1:
        {
            _cpu->GetCurrentStatusRegister().Flags.C = Rm[Imm == 0 ? 31 : (Imm - 1)];
            if (Imm == 0)
                Rd = 0;
            else
                Rd = Rm << Imm;
            break;
        }
        case Thumb::ThumbOpcodes::ASR_1:
        {
            _cpu->GetCurrentStatusRegister().Flags.C = Rm[Imm == 0 ? 31 : (Imm - 1)];
            if (Imm == 0)
                Rd = (Rm[31] ? 0 : 0xFFFFFFFF);
            else
                Rd = int32_t(Rm) >> 5;
            break;
        }
    }
    _cpu->GetCurrentStatusRegister().Flags.N = Rd[31];
    _cpu->GetCurrentStatusRegister().Flags.Z = Rd != 0;
}

void Interpreter::HandleThumbAddSubImmRegInstruction(std::shared_ptr<ThumbInstruction> instruction)
{
    auto instr = std::static_pointer_cast<Thumb::AddSubInstruction>(instruction);
    GeneralPurposeRegister& Rd = _cpu->GetRegister(instr->GetDestinationRegister());
    GeneralPurposeRegister& Rn = _cpu->GetRegister(instr->GetSourceRegister());
    switch (instr->GetOpcode())
    {
        case Thumb::ThumbOpcodes::ADD_1:
        case Thumb::ThumbOpcodes::ADD_3:
            Rd = Rn + (instr->IsImmediate() ? instr->GetThirdOperand() : _cpu->GetRegister(instr->GetThirdOperand()));
            // V Flag = OverflowFrom(Rn + immed_3) | CarryFrom(Rn + Rm)
            // C Flag = CarryFrom(Rn + immed_3) | OverflowFrom(Rn + Rm)
            break;
        case Thumb::ThumbOpcodes::SUB_1:
        case Thumb::ThumbOpcodes::SUB_3:
            Rd = Rn - (instr->IsImmediate() ? instr->GetThirdOperand() : _cpu->GetRegister(instr->GetThirdOperand()));
            // C Flag = NOT BorrowFrom(Rn - immed_3) | NOT BorrowFrom(Rn - Rm)
            // V Flag = OverflowFrom(Rn - immed_3) | OverflowFrom(Rn - Rm)

    }
    _cpu->GetCurrentStatusRegister().Flags.N = Rd[31];
    _cpu->GetCurrentStatusRegister().Flags.N = Rd != 0;
}

void Interpreter::HandleARMMultiplyInstruction(std::shared_ptr<ARMInstruction> instruction)
{
    auto mul = std::static_pointer_cast<ARM::MultiplyAccumulateInstruction>(instruction);
    GeneralPurposeRegister& firstOp = _cpu->GetRegister(mul->GetFirstOperand());
    GeneralPurposeRegister& secondOp = _cpu->GetRegister(mul->GetSecondOperand());
    
    int64_t result = firstOp * secondOp;
    
    if (!mul->IsSigned())
        result = std::abs(result);
    
    int64_t added = 0;

    if (mul->IsLong())
        added = (_cpu->GetRegister(mul->GetDestinationRegisterHigh()) << 31) | _cpu->GetRegister(mul->GetDestinationRegisterLow());
    else
        added = _cpu->GetRegister(mul->GetThirdOperand());
    
    if (mul->Accumulate())
        result += added;

    uint32_t lower = MathHelper::GetBits(result, 0, 32);
    uint32_t higher = MathHelper::GetBits(result, 32, 32);

    if (!mul->IsLong())
        _cpu->GetRegister(mul->GetDestinationRegisterHigh()) = lower;
    else
    {
        _cpu->GetRegister(mul->GetDestinationRegisterLow()) = lower;
        _cpu->GetRegister(mul->GetDestinationRegisterHigh()) = higher;
    }

    if (mul->SetConditionCodes())
    {
        if (mul->IsLong())
        {
            _cpu->GetCurrentStatusFlags().Z = lower == 0 && higher == 0;
            _cpu->GetCurrentStatusFlags().N = MathHelper::CheckBit(higher, 31);
        }
        else
        {
            _cpu->GetCurrentStatusFlags().Z = lower == 0;
            _cpu->GetCurrentStatusFlags().N = MathHelper::CheckBit(lower, 31);
        }
        // The Carry flag should be now be UNPREDICTABLE, but we do not handle that.
    }
}

void Interpreter::HandleARMLoadStoreMultipleInstruction(std::shared_ptr<ARMInstruction> instr)
{
    auto instruction = std::static_pointer_cast<ARM::LoadStoreInstruction>(instr);
    
    uint16_t registers = instruction->GetRegistersList();
    uint32_t startAddress = 0;
    uint32_t endAddress = 0;

    // Indicates how the memory will be loaded
    if (instruction->IsBaseAdded())
    {
        startAddress = _cpu->GetRegister(instruction->GetBaseRegister());

        if (instruction->GetBaseRegister() == PC)
            startAddress += 4;

        endAddress = startAddress + MathHelper::NumberOfSetBits(registers) * 4;

        if (!instruction->IsPreIndexed())
            endAddress -= 4;
        else
            startAddress += 4;
    }
    else
    {
        endAddress = _cpu->GetRegister(instruction->GetBaseRegister());

        if (instruction->GetBaseRegister() == PC)
            endAddress += 4;

        startAddress = endAddress - MathHelper::NumberOfSetBits(registers) * 4;

        if (!instruction->IsPreIndexed())
            startAddress += 4;
        else
            endAddress -= 4;
    }

    uint32_t currentAddress = startAddress;
    for (uint8_t i = 0; i < 16 && currentAddress <= endAddress; ++i)
    {
        // If the bit is set, load the data into the register
        if (MathHelper::CheckBit(registers, i))
        {
            if (instruction->IsLoad())
                _cpu->GetRegister(i) = _cpu->GetMemory()->ReadUInt32(currentAddress);
            else
            {
                uint32_t val = _cpu->GetRegister(i);
                if (i == PC)
                    val += 4;
                _cpu->GetMemory()->WriteUInt32(currentAddress, val);
            }
            currentAddress += 4;
        }
    }

    if (instruction->WriteBack() && _cpu->ConditionPasses(instruction->GetCondition()))
    {
        if (instruction->IsBaseAdded())
            _cpu->GetRegister(instruction->GetBaseRegister()) += MathHelper::NumberOfSetBits(registers) * 4;
        else
            _cpu->GetRegister(instruction->GetBaseRegister()) -= MathHelper::NumberOfSetBits(registers) * 4;
    }
}
