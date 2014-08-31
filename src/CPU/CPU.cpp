#include "CPU.hpp"
#include "Memory/Memory.hpp"
#include "Common/MathHelper.hpp"

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
    _gpu = std::unique_ptr<GPU>(new GPU(this));
    _dma = std::unique_ptr<DMA>(new DMA(this));

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
        _dma->Step();
        StepInstruction();
        GetGPU()->Step(_cycles);
        // Check for interrupts on every loop
        ProcessInterrupts();
    }
}

bool CPU::ConditionPasses(InstructionCondition condition)
{
    switch (condition)
    {
        case InstructionCondition::Equal:
            return _state.CPSR.Flags.Z;
        case InstructionCondition::NotEqual:
            return !_state.CPSR.Flags.Z;
        case InstructionCondition::CarrySet:
            return _state.CPSR.Flags.C;
        case InstructionCondition::CarryCleared:
            return !_state.CPSR.Flags.C;
        case InstructionCondition::Negative:
            return _state.CPSR.Flags.N;
        case InstructionCondition::PositiveOrZero:
            return !_state.CPSR.Flags.N;
        case InstructionCondition::Overflow:
            return _state.CPSR.Flags.V;
        case InstructionCondition::NotOverflow:
            return !_state.CPSR.Flags.V;
        case InstructionCondition::UnsignedHigher:
            return _state.CPSR.Flags.C && !_state.CPSR.Flags.Z;
        case InstructionCondition::UnsignedLowerOrSame:
            return !_state.CPSR.Flags.C || _state.CPSR.Flags.Z;
        case InstructionCondition::GreaterEqual:
            return _state.CPSR.Flags.N == _state.CPSR.Flags.V;
        case InstructionCondition::LessThan:
            return _state.CPSR.Flags.N != _state.CPSR.Flags.V;
        case InstructionCondition::GreaterThan:
            return !_state.CPSR.Flags.Z && _state.CPSR.Flags.N == _state.CPSR.Flags.V;
        case InstructionCondition::LessOrEqual:
            return _state.CPSR.Flags.Z || _state.CPSR.Flags.N != _state.CPSR.Flags.V;
        case InstructionCondition::Always:
        case InstructionCondition::Unused:
        default:
            return true;
    }

    return false;
}

void CPU::LoadROM(GBAHeader& header, FILE* rom, FILE* bios)
{
    _memory->LoadROM(header, rom, bios);
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
        _cycles += instruction->GetTiming();
        ExecuteInstructionCallback(InstructionCallbackTypes::InstructionExecuted, instruction);
    }
    else
        std::cout << "Unknown Instruction" << std::endl;
}

bool CPU::IsInterruptEnabled(InterruptTypes type)
{
    // All interrupts are disabled when an IRQ is being handled
    if (GetCurrentStatusFlags().I)
        return false;

    uint32_t masterEnable = GetMemory()->ReadUInt32(InterruptMasterEnableRegister);
    
    // If the Interrupt Master Enable Register says interrupts are disabled, then don't try to check any further
    if (!MathHelper::CheckBit(masterEnable, 0))
        return false;

    uint16_t interrupts = GetMemory()->ReadUInt16(InterruptEnableRegister);
    
    // Check if the specified type of interrupt is disabled in the Interrupt Enable Register
    return MathHelper::CheckBit(interrupts, uint8_t(type));
}

void CPU::TriggerInterrupt(InterruptTypes type)
{
    std::swap(GetRegisterForMode(CPUMode::IRQ, SP), GetRegister(SP));
    std::swap(GetRegisterForMode(CPUMode::IRQ, LR), GetRegister(LR));

    // Save the CPSR into SPSR_irq
    _state.SPSR[2] = GetCurrentStatusRegister();
    
    // Disable interrupts
    GetCurrentStatusFlags().I = 1;

    // Setup the return address
    GetRegisterForMode(CPUMode::IRQ, LR) = PC + (GetCurrentInstructionSet() == InstructionSet::ARM ? 4 : 2);

    // Set IRQ mode
    SetCurrentCPUMode(CPUMode::IRQ);
    
    // Switch to ARM mode
    SetInstructionSet(InstructionSet::ARM);

    // Jump to the BIOS IRQ handler
    GetRegister(PC) = 0x18;
}

void CPU::RequestInterrupt(InterruptTypes type)
{
    if (!IsInterruptEnabled(type))
        return;

    // Write the interrupt to the Interrupt Request Flags, they will be processed on the next tick
    uint16_t interruptRequests = GetMemory()->ReadUInt16(InterruptRequestFlags);
    interruptRequests |= (1 << uint8_t(type));
    GetMemory()->WriteUInt16(InterruptRequestFlags, interruptRequests);
}

void CPU::ProcessInterrupts()
{
    // All interrupts are disabled when an IRQ is being handled
    if (GetCurrentStatusFlags().I)
        return;

    // Check the interrupt flags and service the requested interrupts
    uint16_t interruptRequests = GetMemory()->ReadUInt16(InterruptRequestFlags);
    
    // Only one interrupt is triggered at a time
    if (MathHelper::CheckBit(interruptRequests, 0))
        TriggerInterrupt(InterruptTypes::VBlank);
    else if (MathHelper::CheckBit(interruptRequests, 1))
        TriggerInterrupt(InterruptTypes::HBlank);
    else if (MathHelper::CheckBit(interruptRequests, 2))
        TriggerInterrupt(InterruptTypes::VCounterMatch);
    else if (MathHelper::CheckBit(interruptRequests, 3))
        TriggerInterrupt(InterruptTypes::Timer0Overflow);
    else if (MathHelper::CheckBit(interruptRequests, 4))
        TriggerInterrupt(InterruptTypes::Timer1Overflow);
    else if (MathHelper::CheckBit(interruptRequests, 5))
        TriggerInterrupt(InterruptTypes::Timer2Overflow);
    else if (MathHelper::CheckBit(interruptRequests, 6))
        TriggerInterrupt(InterruptTypes::Timer3Overflow);
    else if (MathHelper::CheckBit(interruptRequests, 7))
        TriggerInterrupt(InterruptTypes::SerialCommunication);
    else if (MathHelper::CheckBit(interruptRequests, 8))
        TriggerInterrupt(InterruptTypes::DMA0);
    else if (MathHelper::CheckBit(interruptRequests, 9))
        TriggerInterrupt(InterruptTypes::DMA1);
    else if (MathHelper::CheckBit(interruptRequests, 10))
        TriggerInterrupt(InterruptTypes::DMA2);
    else if (MathHelper::CheckBit(interruptRequests, 11))
        TriggerInterrupt(InterruptTypes::DMA3);
    else if (MathHelper::CheckBit(interruptRequests, 12))
        TriggerInterrupt(InterruptTypes::KeyPad);
    else if (MathHelper::CheckBit(interruptRequests, 13))
        TriggerInterrupt(InterruptTypes::GamePak);
}
