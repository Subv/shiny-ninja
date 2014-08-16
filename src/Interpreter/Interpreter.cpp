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
}

void Interpreter::HandleARMBranchInstruction(std::shared_ptr<ARMInstruction> instruction)
{
    auto branch = std::static_pointer_cast<ARM::BranchInstruction>(instruction);

    if (branch->Link())
        _cpu->GetRegister(LR) = _cpu->GetRegister(PC);

    // We have to add 4 because the CPU pipeline prefetches the next opcode.
    _cpu->GetRegister(PC) += branch->GetSignedOffset() + 4;
}

