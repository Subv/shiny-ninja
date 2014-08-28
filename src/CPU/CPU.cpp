#include "CPU.hpp"
#include "Memory/Memory.hpp"

#include <iostream>

CPU::CPU(CPUExecutionMode mode) : _mode(mode), _runState(CPURunState::Stopped), 
_decoder(new Decoder())
{
    Reset();
}

void CPU::Reset()
{
    Stop();

    _interpreter = std::unique_ptr<Interpreter>(new Interpreter(this));
    _memory = std::unique_ptr<MMU>(new MMU(this));
    _io = std::unique_ptr<GPU>(new GPU(this));

    // Zero-out all the registers
    _state.Registers = { };
    _state.Registers_FIQ = { };
    _state.Registers_svc = { };
    _state.Registers_abt = { };
    _state.Registers_IRQ = { };
    _state.Registers_und = { };

    _state.CPSR.Full = 0;

    // The GBA boots in System mode
    SetCurrentCPUMode(CPUMode::System);

    GetRegister(PC) = 0x8000000; // Default entry point
}

void CPU::Run()
{
    // Ignore this call if we are already running
    if (_runState == CPURunState::Running)
        return;

    _runState = CPURunState::Running;
    _cycles = 0;

    // Loop until something stops the CPU
    while (_runState == CPURunState::Running)
    {
        StepInstruction();
        GetIO()->Step(_cycles);
    }
}

bool CPU::ConditionPasses(InstructionCondition condition)
{
    // TODO: STUB
    switch (condition)
    {
        default:
            return true;
    }

    return false;
}

void CPU::LoadROM(GBAHeader& header, FILE* rom)
{
    _memory->LoadROM(header, rom);
}

GeneralPurposeRegister& CPU::GetRegister(uint8_t reg)
{
    return GetRegisterForMode(GetCurrentCPUMode(), reg);
}

ProgramStatusRegisters& CPU::GetSavedStatusRegister()
{
    switch (GetCurrentCPUMode())
    {
        case CPUMode::FIQ:
            return _state.SPSR[0];
        case CPUMode::Supervisor:
            return _state.SPSR[1];
        case CPUMode::IRQ:
            return _state.SPSR[2];
        case CPUMode::Abort:
            return _state.SPSR[3];
        case CPUMode::Undefined:
            return _state.SPSR[4];
        default:
            break;
    }

    Utilities::Assert(false, "Current CPU mode doesn't have a SPSR");
    return _state.SPSR[0];
}

void CPU::ExecuteInstructionCallback(InstructionCallbackTypes type, std::shared_ptr<Instruction> instruction)
{
    auto handler = _instructionCallbacks.find(type);
    
    if (handler != _instructionCallbacks.end())
        handler->second(instruction);
}

GeneralPurposeRegister& CPU::GetRegisterForMode(CPUMode mode, uint8_t reg)
{
    Utilities::Assert(reg <= PC, "Trying to access invalid register");

    // R0 through R7, and R15 are shared across all modes
    // User mode and System mode use the same registers
    if (reg <= 7 || reg == PC || mode == CPUMode::User || mode == CPUMode::System)
        return _state.Registers[reg];

    // FIQ mode uses R8_fiq through R14_fiq
    if (mode == CPUMode::FIQ)
        return _state.Registers_FIQ[reg - 8];

    // R8 through R12 are shared in the other modes
    if (reg <= 12)
        return _state.Registers[reg];

    // Registers R13 and R14 are specific to each mode
    if (mode == CPUMode::Supervisor)
        return _state.Registers_svc[reg - 13];
    else if (mode == CPUMode::Abort)
        return _state.Registers_abt[reg - 13];
    else if (mode == CPUMode::IRQ)
        return _state.Registers_IRQ[reg - 13];
    else if (mode == CPUMode::Undefined)
        return _state.Registers_und[reg - 13];

    Utilities::Assert(false, "Unknown CPU mode registers");
    return _state.Registers[reg];
}

void CPU::StepInstruction()
{
    std::shared_ptr<Instruction> instruction;

    if (GetCurrentInstructionSet() == InstructionSet::ARM)
    {
        uint32_t opcode = _memory->ReadUInt32(GetRegister(PC)); // Read the opcode from memory, 4 bytes in ARM mode

        GetRegister(PC) += 4; // Increment the PC 4 bytes

        // Extract the instruction from it
        instruction = _decoder->DecodeARM(opcode);
    }
    else
    {
        uint16_t opcode = _memory->ReadUInt16(GetRegister(PC)); // Read the opcode from memory, 2 bytes in Thumb mode

        GetRegister(PC) += 2; // Increment the PC 2 bytes

        // Extract the instruction from it
        instruction = _decoder->DecodeThumb(opcode);
    }

    ExecuteInstructionCallback(InstructionCallbackTypes::InstructionDecoded, instruction);

    if (instruction)
    {
        _interpreter->RunInstruction(instruction);
        ExecuteInstructionCallback(InstructionCallbackTypes::InstructionExecuted, instruction);
    }
    else
        std::cout << "Unknown Instruction" << std::endl;
}
