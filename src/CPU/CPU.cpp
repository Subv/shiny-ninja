#include "CPU.hpp"
#include "Memory/Memory.hpp"

CPU::CPU(CPUMode mode) : _mode(mode), _runState(CPURunState::Stopped),
_decoder(new Decoder()), _interpreter(new Interpreter())
{
    Reset();
}

void CPU::Reset()
{

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
        // Read the opcode from memory
        // TODO: Implement the Memory Management Unit
        uint32_t opcode = 0x0;

        // Extract the instruction from it
        // TODO: Only ARM mode is supported for now
        Instruction instruction = _decoder->DecodeARM(opcode);

        _interpreter->RunInstruction(instruction);
    }
}

void CPU::LoadROM(GBAHeader& header, FILE* rom)
{
    sMemory->LoadROM(header, rom);
}
