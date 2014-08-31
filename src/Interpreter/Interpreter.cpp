#include "Interpreter.hpp"

#include "CPU/CPU.hpp"

#include "Common/Instructions/ARMInstruction.hpp"
#include "Common/Instructions/ThumbInstruction.hpp"

#include "Common/Instructions/Thumb/MiscInstructions.hpp"
#include "Common/Instructions/Thumb/DataProcessingInstructions.hpp"

#include "Common/MathHelper.hpp"
#include "Common/Utilities.hpp"

Interpreter::Interpreter(CPU* arm) : _cpu(arm)
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
    auto handler = _thumbHandlers.find(Thumb::ThumbOpcodes(instruction->GetOpcode()));

    if (handler != _thumbHandlers.end())
        handler->second(instruction);
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
    _armHandlers[ARM::ARMOpcodes::LDRBT] = std::bind(&Interpreter::HandleARMLoadStoreInstruction, this, std::placeholders::_1);
    _armHandlers[ARM::ARMOpcodes::STR] = std::bind(&Interpreter::HandleARMLoadStoreInstruction, this, std::placeholders::_1);
    _armHandlers[ARM::ARMOpcodes::STRB] = std::bind(&Interpreter::HandleARMLoadStoreInstruction, this, std::placeholders::_1);
    _armHandlers[ARM::ARMOpcodes::STRBT] = std::bind(&Interpreter::HandleARMLoadStoreInstruction, this, std::placeholders::_1);

    // Miscellaneous Load / Store instructions
    _armHandlers[ARM::ARMOpcodes::STRH] = std::bind(&Interpreter::HandleARMMiscellaneousLoadStoreInstruction, this, std::placeholders::_1);
    _armHandlers[ARM::ARMOpcodes::LDRH] = std::bind(&Interpreter::HandleARMMiscellaneousLoadStoreInstruction, this, std::placeholders::_1);
    _armHandlers[ARM::ARMOpcodes::LDRSH] = std::bind(&Interpreter::HandleARMMiscellaneousLoadStoreInstruction, this, std::placeholders::_1);
    _armHandlers[ARM::ARMOpcodes::LDRSB] = std::bind(&Interpreter::HandleARMMiscellaneousLoadStoreInstruction, this, std::placeholders::_1);

    // Not Yet Implemented
    _armHandlers[ARM::ARMOpcodes::LDM] = std::bind(&Interpreter::HandleARMLoadStoreMultipleInstruction, this, std::placeholders::_1);
    _armHandlers[ARM::ARMOpcodes::STM] = std::bind(&Interpreter::HandleARMLoadStoreMultipleInstruction, this, std::placeholders::_1);

    // PSR Operations
    _armHandlers[ARM::ARMOpcodes::MRS] = std::bind(&Interpreter::HandleARMPSROperationInstruction, this, std::placeholders::_1);
    _armHandlers[ARM::ARMOpcodes::MSR] = std::bind(&Interpreter::HandleARMPSROperationInstruction, this, std::placeholders::_1);

    // Multiply and Multiply Accumulate
    _armHandlers[ARM::ARMOpcodes::MUL] = std::bind(&Interpreter::HandleARMMultiplyInstruction, this, std::placeholders::_1);
    _armHandlers[ARM::ARMOpcodes::MLA] = std::bind(&Interpreter::HandleARMMultiplyInstruction, this, std::placeholders::_1);
    _armHandlers[ARM::ARMOpcodes::SMLAL] = std::bind(&Interpreter::HandleARMMultiplyInstruction, this, std::placeholders::_1);
    _armHandlers[ARM::ARMOpcodes::SMULL] = std::bind(&Interpreter::HandleARMMultiplyInstruction, this, std::placeholders::_1);
    _armHandlers[ARM::ARMOpcodes::UMLAL] = std::bind(&Interpreter::HandleARMMultiplyInstruction, this, std::placeholders::_1);
    _armHandlers[ARM::ARMOpcodes::UMULL] = std::bind(&Interpreter::HandleARMMultiplyInstruction, this, std::placeholders::_1);
}

void Interpreter::InitializeThumb()
{
    // Stack operations
    _thumbHandlers[Thumb::ThumbOpcodes::PUSH] = std::bind(&Interpreter::HandleThumbStackOperationInstruction, this, std::placeholders::_1);
    _thumbHandlers[Thumb::ThumbOpcodes::POP] = std::bind(&Interpreter::HandleThumbStackOperationInstruction, this, std::placeholders::_1);

    // Immediate Shift operations
    _thumbHandlers[Thumb::ThumbOpcodes::LSL_1] = std::bind(&Interpreter::HandleThumbImmediateShiftInstruction, this, std::placeholders::_1);
    _thumbHandlers[Thumb::ThumbOpcodes::LSR_1] = std::bind(&Interpreter::HandleThumbImmediateShiftInstruction, this, std::placeholders::_1);
    _thumbHandlers[Thumb::ThumbOpcodes::ASR_1] = std::bind(&Interpreter::HandleThumbImmediateShiftInstruction, this, std::placeholders::_1);

    // Add/Substract Register/Immediate operations
    _thumbHandlers[Thumb::ThumbOpcodes::ADD_1] = std::bind(&Interpreter::HandleThumbAddSubImmRegInstruction, this, std::placeholders::_1);
    _thumbHandlers[Thumb::ThumbOpcodes::ADD_3] = std::bind(&Interpreter::HandleThumbAddSubImmRegInstruction, this, std::placeholders::_1);
    _thumbHandlers[Thumb::ThumbOpcodes::SUB_1] = std::bind(&Interpreter::HandleThumbAddSubImmRegInstruction, this, std::placeholders::_1);
    _thumbHandlers[Thumb::ThumbOpcodes::SUB_3] = std::bind(&Interpreter::HandleThumbAddSubImmRegInstruction, this, std::placeholders::_1);

    // Add/Sub/Cmp/Mov Immediate operations
    _thumbHandlers[Thumb::ThumbOpcodes::ADD_2] = std::bind(&Interpreter::HandleThumbAddCmpMovSubImmediateInstruction, this, std::placeholders::_1);
    _thumbHandlers[Thumb::ThumbOpcodes::SUB_2] = std::bind(&Interpreter::HandleThumbAddCmpMovSubImmediateInstruction, this, std::placeholders::_1);
    _thumbHandlers[Thumb::ThumbOpcodes::MOV_1] = std::bind(&Interpreter::HandleThumbAddCmpMovSubImmediateInstruction, this, std::placeholders::_1);
    _thumbHandlers[Thumb::ThumbOpcodes::CMP_1] = std::bind(&Interpreter::HandleThumbAddCmpMovSubImmediateInstruction, this, std::placeholders::_1);

    // Data Processing Register operations
    _thumbHandlers[Thumb::ThumbOpcodes::AND] = std::bind(&Interpreter::HandleThumbDataProcessingInstruction, this, std::placeholders::_1);
    _thumbHandlers[Thumb::ThumbOpcodes::EOR] = std::bind(&Interpreter::HandleThumbDataProcessingInstruction, this, std::placeholders::_1);
    _thumbHandlers[Thumb::ThumbOpcodes::LSL_2] = std::bind(&Interpreter::HandleThumbDataProcessingInstruction, this, std::placeholders::_1);
    _thumbHandlers[Thumb::ThumbOpcodes::LSR_2] = std::bind(&Interpreter::HandleThumbDataProcessingInstruction, this, std::placeholders::_1);
    _thumbHandlers[Thumb::ThumbOpcodes::ASR_2] = std::bind(&Interpreter::HandleThumbDataProcessingInstruction, this, std::placeholders::_1);
    _thumbHandlers[Thumb::ThumbOpcodes::ADC] = std::bind(&Interpreter::HandleThumbDataProcessingInstruction, this, std::placeholders::_1);
    _thumbHandlers[Thumb::ThumbOpcodes::SBC] = std::bind(&Interpreter::HandleThumbDataProcessingInstruction, this, std::placeholders::_1);
    _thumbHandlers[Thumb::ThumbOpcodes::ROR] = std::bind(&Interpreter::HandleThumbDataProcessingInstruction, this, std::placeholders::_1);
    _thumbHandlers[Thumb::ThumbOpcodes::TST] = std::bind(&Interpreter::HandleThumbDataProcessingInstruction, this, std::placeholders::_1);
    _thumbHandlers[Thumb::ThumbOpcodes::NEG] = std::bind(&Interpreter::HandleThumbDataProcessingInstruction, this, std::placeholders::_1);
    _thumbHandlers[Thumb::ThumbOpcodes::CMP_2] = std::bind(&Interpreter::HandleThumbDataProcessingInstruction, this, std::placeholders::_1);
    _thumbHandlers[Thumb::ThumbOpcodes::CMN] = std::bind(&Interpreter::HandleThumbDataProcessingInstruction, this, std::placeholders::_1);
    _thumbHandlers[Thumb::ThumbOpcodes::ORR] = std::bind(&Interpreter::HandleThumbDataProcessingInstruction, this, std::placeholders::_1);
    _thumbHandlers[Thumb::ThumbOpcodes::MUL] = std::bind(&Interpreter::HandleThumbDataProcessingInstruction, this, std::placeholders::_1);
    _thumbHandlers[Thumb::ThumbOpcodes::BIC] = std::bind(&Interpreter::HandleThumbDataProcessingInstruction, this, std::placeholders::_1);

    // Special Data Processing Register operations
    _thumbHandlers[Thumb::ThumbOpcodes::ADD_4] = std::bind(&Interpreter::HandleThumbSpecialDataProcessingInstruction, this, std::placeholders::_1);
    _thumbHandlers[Thumb::ThumbOpcodes::CMP_3] = std::bind(&Interpreter::HandleThumbSpecialDataProcessingInstruction, this, std::placeholders::_1);
    _thumbHandlers[Thumb::ThumbOpcodes::MOV_3] = std::bind(&Interpreter::HandleThumbSpecialDataProcessingInstruction, this, std::placeholders::_1);

    // Branch/Exchange Instruction operations
    _thumbHandlers[Thumb::ThumbOpcodes::BX] = std::bind(&Interpreter::HandleThumbBranchExchangeInstruction, this, std::placeholders::_1);
    _thumbHandlers[Thumb::ThumbOpcodes::BLX_2] = std::bind(&Interpreter::HandleThumbBranchExchangeInstruction, this, std::placeholders::_1);

    // Long Branch operation
    _thumbHandlers[Thumb::ThumbOpcodes::BL] = std::bind(&Interpreter::HandleThumbBranchLinkInstruction, this, std::placeholders::_1);

    // Load from literal pool operation
    _thumbHandlers[Thumb::ThumbOpcodes::LDR_3] = std::bind(&Interpreter::HandleThumbLiteralPoolLoadInstruction, this, std::placeholders::_1);

    // Load/Store Register Offset operation
    _thumbHandlers[Thumb::ThumbOpcodes::LDR_2] = std::bind(&Interpreter::HandleThumbLoadStoreRegisterOffsetInstruction, this, std::placeholders::_1);
    _thumbHandlers[Thumb::ThumbOpcodes::LDRSB] = std::bind(&Interpreter::HandleThumbLoadStoreRegisterOffsetInstruction, this, std::placeholders::_1);
    _thumbHandlers[Thumb::ThumbOpcodes::LDRSH] = std::bind(&Interpreter::HandleThumbLoadStoreRegisterOffsetInstruction, this, std::placeholders::_1);
    _thumbHandlers[Thumb::ThumbOpcodes::LDRH_2] = std::bind(&Interpreter::HandleThumbLoadStoreRegisterOffsetInstruction, this, std::placeholders::_1);
    _thumbHandlers[Thumb::ThumbOpcodes::LDRB_2] = std::bind(&Interpreter::HandleThumbLoadStoreRegisterOffsetInstruction, this, std::placeholders::_1);
    _thumbHandlers[Thumb::ThumbOpcodes::STR_2] = std::bind(&Interpreter::HandleThumbLoadStoreRegisterOffsetInstruction, this, std::placeholders::_1);
    _thumbHandlers[Thumb::ThumbOpcodes::STRB_2] = std::bind(&Interpreter::HandleThumbLoadStoreRegisterOffsetInstruction, this, std::placeholders::_1);
    _thumbHandlers[Thumb::ThumbOpcodes::STRH_2] = std::bind(&Interpreter::HandleThumbLoadStoreRegisterOffsetInstruction, this, std::placeholders::_1);

    // Load/Store Word/Byte/Halfword Immediate Offset operation
    _thumbHandlers[Thumb::ThumbOpcodes::LDR_4] = std::bind(&Interpreter::HandleThumbLoadStoreStackInstruction, this, std::placeholders::_1);
    _thumbHandlers[Thumb::ThumbOpcodes::STR_3] = std::bind(&Interpreter::HandleThumbLoadStoreStackInstruction, this, std::placeholders::_1);

    // Load/Store Multiple operation
    _thumbHandlers[Thumb::ThumbOpcodes::STMIA] = std::bind(&Interpreter::HandleThumbLoadStoreMultipleInstruction, this, std::placeholders::_1);
    _thumbHandlers[Thumb::ThumbOpcodes::LDMIA] = std::bind(&Interpreter::HandleThumbLoadStoreMultipleInstruction, this, std::placeholders::_1);
}