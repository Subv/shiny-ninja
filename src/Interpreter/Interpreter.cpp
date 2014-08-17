#include "Interpreter.hpp"

#include "CPU/CPU.hpp"

#include "Common/Instructions/ARMInstruction.hpp"
#include "Common/Instructions/ThumbInstruction.hpp"

#include "Common/Instructions/ARM/BranchInstructions.hpp"
#include "Common/Instructions/ARM/DataProcessingInstructions.hpp"

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
            bool thumb = MathHelper::CheckBit(0, reg);

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

    uint32_t firstOperand = _cpu->GetRegister(dataproc->GetFirstOperand());
    
    // Account for CPU prefetch, the code expects the PC to be at <CurrentInstruction> + 8, but we're currently at <CurrentInstruction> + 4
    if (dataproc->GetFirstOperand() == PC)
        firstOperand += 4;

    uint32_t secondOperand = 0;
    uint32_t result = 0;

    if (dataproc->IsImmediate())
        secondOperand = dataproc->GetShiftedSecondOperandImmediate();
    else
    {
        uint32_t registerValue = _cpu->GetRegister(dataproc->GetSecondOperand());
        // If the instruction must shift the second operand by a register value, then only the lower 8 bits of that register are used.
        uint8_t shiftValue = dataproc->ShiftByRegister() ? (_cpu->GetRegister(dataproc->GetShiftRegisterOrImmediate()) & 0xFF) : dataproc->GetShiftRegisterOrImmediate();

        switch (dataproc->GetShiftType())
        {
            case ARM::ShiftType::LSL:
                secondOperand = registerValue << shiftValue;
                break;
            case ARM::ShiftType::LSR:
                secondOperand = registerValue >> shiftValue;
                break;
            case ARM::ShiftType::ASR:
                // We need to convert it to int32_t in order to perform an Arithmetic Shift Right (ASR)
                secondOperand = uint32_t((int32_t)registerValue >> (int32_t)shiftValue);
                break;
            case ARM::ShiftType::ROR:
                secondOperand = MathHelper::RotateRight(registerValue, shiftValue);
                break;
        }

    }

    switch (dataproc->GetOpcode())
    {
        case ARM::ARMOpcodes::AND:
            result = firstOperand & secondOperand;
            break;
        case ARM::ARMOpcodes::EOR:
            result = firstOperand ^ secondOperand;
            break;
        case ARM::ARMOpcodes::SUB:
            result = firstOperand - secondOperand;
            break;
        case ARM::ARMOpcodes::RSB: // Reversed Subtract
            result = secondOperand - firstOperand;
            break;
        case ARM::ARMOpcodes::ADD:
            result = firstOperand + secondOperand;
            break;
        case ARM::ARMOpcodes::ADC: // Add with carry
            result = firstOperand + secondOperand;
            break;
    }

    if (dataproc->HasDestinationRegister())
        _cpu->GetRegister(dataproc->GetDestinationRegister()) = result;
}

