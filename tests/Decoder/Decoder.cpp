#include "catch/catch.hpp"
#include "Decoder/Decoder.hpp"
#include "Common/Instructions/ARM/BranchInstruction.hpp"

TEST_CASE("Decoder", "Tests that the decoder is correctly identifying instructions")
{
    Decoder* decoder = new Decoder();
    std::shared_ptr<Instruction> instruction = decoder->DecodeARM(0xEA00002E);

    // Verify that the instruction is correctly decoded
    REQUIRE(std::dynamic_pointer_cast<ARM::BranchInstruction>(instruction));

    instruction = decoder->DecodeARM(0xE12FFF10);

    REQUIRE(std::dynamic_pointer_cast<ARM::BranchLinkExchangeRegisterInstruction>(instruction));
}