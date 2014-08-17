#include "Interpreter.hpp"

#include "CPU/CPU.hpp"

#include "Common/Instructions/ARMInstruction.hpp"
#include "Common/Instructions/ThumbInstruction.hpp"

#include "Common/Instructions/ARM/BranchInstructions.hpp"
#include "Common/Instructions/ARM/DataProcessingInstructions.hpp"
#include "Common/Instructions/ARM/LoadStoreInstructions.h"

#include "Common/MathHelper.hpp"
#include "Common/Utilities.hpp"

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
    _armHandlers[ARM::ARMOpcodes::STR] = std::bind(&Interpreter::HandleARMLoadStoreInstruction, this, std::placeholders::_1);
    _armHandlers[ARM::ARMOpcodes::STRB] = std::bind(&Interpreter::HandleARMLoadStoreInstruction, this, std::placeholders::_1);
    _armHandlers[ARM::ARMOpcodes::LDM] = std::bind(&Interpreter::HandleARMLoadStoreInstruction, this, std::placeholders::_1);
    _armHandlers[ARM::ARMOpcodes::STM] = std::bind(&Interpreter::HandleARMLoadStoreInstruction, this, std::placeholders::_1);
}

void Interpreter::InitializeThumb()
{
    
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
    uint32_t secondAddressValue = 0;

    if (instruction->IsImmediate())
        secondAddressValue = instruction->GetImmediateOffset();
    else
    {
        int32_t registerValue = _cpu->GetRegister(instruction->GetIndexRegister());
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

        if (instruction->WriteBack() && _cpu->ConditionPasses(instruction->GetCondition))
            _cpu->GetRegister(instruction->GetBaseRegister()) = address;
    }
    else
    {
        if (instruction->WriteBack() && _cpu->ConditionPasses(instruction->GetCondition))
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

            break;
    }
}

