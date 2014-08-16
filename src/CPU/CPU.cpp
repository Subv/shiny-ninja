#include "CPU.hpp"
#include "Memory/Memory.hpp"

#include <iostream>

CPU::CPU(CPUMode mode) : _mode(mode), _runState(CPURunState::Stopped),
_interpreter(new Interpreter(shared_from_this())), _decoder(new Decoder())
{
    Reset();
}

void CPU::Reset()
{
    for (int i = 0; i < 16; ++i)
        _state.Registers[i] = 0;

    PC = 0x8000000; // Default entry point.

    _state.CPSR.Full = 0;
}

void CPU::Run()
{
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
            uint32_t opcode = sMemory->Read32(PC);

            PC += 4; // Increment the PC 4 bytes

            // Extract the instruction from it
            instruction = _decoder->DecodeARM(opcode);
        }
        else
        {
            // Read the opcode from memory, 2 bytes in Thumb mode
            uint16_t opcode = sMemory->Read16(PC);

            PC += 2; // Increment the PC 2 bytes

            // Extract the instruction from it
            instruction = _decoder->DecodeThumb(opcode);
        }

        std::cout << "Instruction: " << instruction->ToString() << std::endl;

        _interpreter->RunInstruction(instruction);
    }
}

void CPU::LoadROM(GBAHeader& header, FILE* rom)
{
    sMemory->LoadROM(header, rom);
}
