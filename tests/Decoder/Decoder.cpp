#include "catch/catch.hpp"
#include "Decoder/Decoder.hpp"
#include "Common/Instructions/ARM/BranchInstruction.hpp"

TEST_CASE("Decoder", "Tests that the decoder is correctly identifying instructions")
{
    Decoder* decoder = new Decoder();
    std::shared_ptr<Instruction> instruction = decoder->DecodeARM(0xEA00002E);

    // Verify that the instruction is correctly decoded
    REQUIRE(std::dynamic_pointer_cast<ARM::BranchInstruction>(instruction));

    // Perform some validity checks
    REQUIRE(instruction->GetOpcode() == ARMOpcodes::B);
    REQUIRE(std::static_pointer_cast<ARMInstruction>(instruction)->GetCondition() == InstructionCondition::Always);

    instruction = decoder->DecodeARM(0xE12FFF10);

    auto bx = std::dynamic_pointer_cast<ARM::BranchLinkExchangeRegisterInstruction>(instruction);
    REQUIRE(bx);
    REQUIRE(bx->GetOpcode() == ARMOpcodes::BX);
    REQUIRE(bx->GetCondition() == InstructionCondition::Always);
    REQUIRE(bx->GetRegister() == 0);
    REQUIRE(bx->Link() == false);

    instruction = decoder->DecodeARM(0xF83CF000);

    auto bl = std::dynamic_pointer_cast<ARM::BranchInstruction>(instruction);
    REQUIRE(bl);
    REQUIRE(bl->GetOpcode() == ARMOpcodes::BL);
    REQUIRE(bl->GetCondition() == InstructionCondition::Always);
    REQUIRE(bl->GetSignedOffset() == 0x7C - 8);
    REQUIRE(bl->Link());
}