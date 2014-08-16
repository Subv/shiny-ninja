#include "Interpreter.hpp"

#include "Common/Instructions/ARMInstruction.hpp"
#include "Common/Instructions/ThumbInstruction.hpp"

#include "Common/Utilities.hpp"

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
    auto handler = _armHandlers[ARM::ARMOpcodes(instruction->GetOpcode())];
}

void Interpreter::HandleThumb(std::shared_ptr<ThumbInstruction> instruction)
{

}

void Interpreter::InitializeHandlers()
{
    _armHandlers[ARM::ARMOpcodes::B] = std::bind(&Interpreter::HandleARMBranchInstruction, this, std::placeholders::_1);
}

void Interpreter::HandleARMBranchInstruction(std::shared_ptr<ARMInstruction> instruction)
{

}

