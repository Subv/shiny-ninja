#include "CPU.hpp"
#include "Memory/Memory.hpp"

#include <iostream>

CPU::CPU(CPUMode mode) : _mode(mode), _runState(CPURunState::Stopped), 
_decoder(new Decoder())
{
    Reset();
}

void CPU::Reset()
{
    for (int i = 0; i < 16; ++i)
        _state.Registers[i] = 0;

    GetRegister(PC) = 0x8000000; // Default entry point.

    _state.CPSR.Full = 0;
}

void CPU::Run()
{
    // These are here because shared_from_this() will throw if called from the constructor
    _interpreter = std::unique_ptr<Interpreter>(new Interpreter(shared_from_this()));
    _memory = std::unique_ptr<MMU>(new MMU(shared_from_this()));

    // Ignore this call if we are already running
    if (_runState == CPURunState::Running)
        return;

    _runState = CPURunState::Running;

    // Loop until something stops the CPU
    while (_runState == CPURunState::Running)
    {
        std::shared_ptr<Instruction> instruction;

        if (GetCurrentInstructionSet() == InstructionSet::ARM)
        {
            // Read the opcode from memory, 4 bytes in ARM mode
            uint32_t opcode = _memory->ReadInt32(GetRegister(PC));

            GetRegister(PC) += 4; // Increment the PC 4 bytes

            // Extract the instruction from it
            instruction = _decoder->DecodeARM(opcode);
        }
        else
        {
            // Read the opcode from memory, 2 bytes in Thumb mode
            uint16_t opcode = _memory->ReadInt16(GetRegister(PC));

            GetRegister(PC) += 2; // Increment the PC 2 bytes

            // Extract the instruction from it
            instruction = _decoder->DecodeThumb(opcode);
        }

        std::cout << "Set: " << uint32_t(instruction->GetInstructionSet()) << ". Instruction: " << instruction->ToString() << std::endl;

        _interpreter->RunInstruction(instruction);
    }
}

void CPU::LoadROM(GBAHeader& header, FILE* rom)
{
    _memory->LoadROM(header, rom);
}
