#include "Interpreter.hpp"

#include "CPU/CPU.hpp"

#include "Common/Instructions/ARMInstruction.hpp"
#include "Common/Instructions/ThumbInstruction.hpp"

#include "Common/Instructions/ARM/BranchInstructions.hpp"

#include "Common/Utilities.hpp"

Interpreter::Interpreter(std::shared_ptr<CPU> arm)  : _cpu(arm)
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
    _armHandlers[ARM::ARMOpcodes::B] = std::bind(&Interpreter::HandleARMBranchInstruction, this, std::placeholders::_1);
    _armHandlers[ARM::ARMOpcodes::BL] = std::bind(&Interpreter::HandleARMBranchInstruction, this, std::placeholders::_1);
    _armHandlers[ARM::ARMOpcodes::BLX] = std::bind(&Interpreter::HandleARMBranchInstruction, this, std::placeholders::_1);
}

void Interpreter::HandleARMBranchInstruction(std::shared_ptr<ARMInstruction> instruction)
{
    if (instruction->GetOpcode() == ARMOpcodes::BLX || instruction->GetOpcode() == ARMOpcodes::BX)
    {
        // Save the return address, only the BLX instruction does this
        if (instruction->GetOpcode() == ARMOpcodes::BLX)
            _cpu->GetRegister(LR) = _cpu->GetRegister(PC);

        if (instruction->IsImmediate())
        {
            auto branch = std::static_pointer_cast<ARM::BranchLinkExchangeImmediateInstruction>(instruction);
            // The second bit of the signed offset is set to the value specified in the opcode (GetSecondBit())
            _cpu->GetRegister(PC) += branch->GetSignedOffset() + (branch->GetSecondBit() << 1);

            // This version of BLX always switches to Thumb mode
            _cpu->SetInstructionSet(InstructionSet::Thumb);
        }
        else
        {
            auto branch = std::static_pointer_cast<ARM::BranchLinkExchangeRegisterInstruction>(instruction);

            // The value of the T bit will be that of the first bit of the value stored in the specified register
            uint32_t reg = _cpu->GetRegister(branch->GetRegister());
            bool thumb = MathHelper::CheckBit(0, reg);

            // The branch offset is stored in a register, with the first bit set to 0
            _cpu->GetRegister(PC) += reg & 0xFFFFFFFE;

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

