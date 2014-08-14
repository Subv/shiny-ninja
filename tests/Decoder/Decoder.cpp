#include "catch/catch.hpp"
#include "Decoder/Decoder.hpp"
#include "Common/Instructions/ARM/BranchInstructions.hpp"
#include "Common/Instructions/ARM/DataProcessingInstructions.hpp"
#include "Common/Instructions/ARM/PSRTransferInstructions.hpp"

TEST_CASE("Decoder", "Tests that the decoder is correctly identifying instructions")
{
    Decoder* decoder = new Decoder();
    std::shared_ptr<Instruction> instruction = decoder->DecodeARM(0xEA00002E);

    // Verify that the instruction is correctly decoded
    REQUIRE(std::dynamic_pointer_cast<ARM::BranchInstruction>(instruction));

    instruction = decoder->DecodeARM(0xE12FFF10);
    REQUIRE(std::dynamic_pointer_cast<ARM::BranchLinkExchangeRegisterInstruction>(instruction));

    instruction = decoder->DecodeARM(0xE2004020);
    REQUIRE(std::dynamic_pointer_cast<ARM::DataProcessingInstruction>(instruction));

    instruction = decoder->DecodeARM(0xE1100003);
    REQUIRE(std::dynamic_pointer_cast<ARM::DataProcessingInstruction>(instruction));

    instruction = decoder->DecodeARM(0xE10F3000);
    REQUIRE(std::dynamic_pointer_cast<ARM::MovePSRToRegisterInstruction>(instruction));

    instruction = decoder->DecodeARM(0xE328F001);
    REQUIRE(std::dynamic_pointer_cast<ARM::MoveRegisterToPSRImmediateInstruction>(instruction));

    instruction = decoder->DecodeARM(0xE128F003);
    REQUIRE(std::dynamic_pointer_cast<ARM::MoveRegisterToPSRRegisterInstruction>(instruction));

    delete decoder;
}