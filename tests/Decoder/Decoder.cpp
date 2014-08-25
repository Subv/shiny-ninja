#include "catch/catch.hpp"
#include "Decoder/Decoder.hpp"
#include "Common/Instructions/ARM/BranchInstructions.hpp"
#include "Common/Instructions/ARM/DataProcessingInstructions.hpp"
#include "Common/Instructions/ARM/PSRTransferInstructions.hpp"

TEST_CASE("Decoder", "Tests that the decoder is correctly identifying instructions")
{
    Decoder* decoder = new Decoder();

    std::shared_ptr<Instruction> instruction = decoder->DecodeARM(0xEA00002E);
    REQUIRE(std::dynamic_pointer_cast<ARM::BranchInstruction>(instruction));

    // BL label1
    instruction = decoder->DecodeARM(0xEBFFFFFE);
    REQUIRE(std::dynamic_pointer_cast<ARM::BranchInstruction>(instruction));
    
    // BX r1
    instruction = decoder->DecodeARM(0xE12FFF11);
    REQUIRE(std::dynamic_pointer_cast<ARM::BranchLinkExchangeRegisterInstruction>(instruction));

    // BLX r3
    instruction = decoder->DecodeARM(0xE12FFF33);
    REQUIRE(std::dynamic_pointer_cast<ARM::BranchLinkExchangeRegisterInstruction>(instruction));

    // B label2
    instruction = decoder->DecodeARM(0xEAFFFFFD);
    REQUIRE(std::dynamic_pointer_cast<ARM::BranchInstruction>(instruction));

    // AND r0, r1, #10
    instruction = decoder->DecodeARM(0xE201000A);
    REQUIRE(std::dynamic_pointer_cast<ARM::DataProcessingInstruction>(instruction));
    // AND r0, r1, r2
    instruction = decoder->DecodeARM(0xE0010002);
    REQUIRE(std::dynamic_pointer_cast<ARM::DataProcessingInstruction>(instruction));
    
    // EOR r0, r1, #10
    instruction = decoder->DecodeARM(0xE221000A);
    REQUIRE(std::dynamic_pointer_cast<ARM::DataProcessingInstruction>(instruction));
    // EOR r0, r1, #10
    instruction = decoder->DecodeARM(0xE021000A);
    REQUIRE(std::dynamic_pointer_cast<ARM::DataProcessingInstruction>(instruction));

    // SUB r0, r1, #10
    instruction = decoder->DecodeARM(0xE241000A);
    REQUIRE(std::dynamic_pointer_cast<ARM::DataProcessingInstruction>(instruction));
    // SUB r0, r1, #10
    instruction = decoder->DecodeARM(0xE041000A);
    REQUIRE(std::dynamic_pointer_cast<ARM::DataProcessingInstruction>(instruction));

    // RSB r0, r1, #10
    instruction = decoder->DecodeARM(0xE261000A);
    REQUIRE(std::dynamic_pointer_cast<ARM::DataProcessingInstruction>(instruction));
    // RSB r0, r1, #10
    instruction = decoder->DecodeARM(0xE061000A);
    REQUIRE(std::dynamic_pointer_cast<ARM::DataProcessingInstruction>(instruction));

    // ADD r0, r1, #10
    instruction = decoder->DecodeARM(0xE281000A);
    REQUIRE(std::dynamic_pointer_cast<ARM::DataProcessingInstruction>(instruction));
    // ADD r0, r1, #10
    instruction = decoder->DecodeARM(0xE081000A);
    REQUIRE(std::dynamic_pointer_cast<ARM::DataProcessingInstruction>(instruction));

    // ADC r0, r1, #10
    instruction = decoder->DecodeARM(0xE2A1000A);
    REQUIRE(std::dynamic_pointer_cast<ARM::DataProcessingInstruction>(instruction));
    // ADC r0, r1, #10
    instruction = decoder->DecodeARM(0xE0A1000A);
    REQUIRE(std::dynamic_pointer_cast<ARM::DataProcessingInstruction>(instruction));

    // SBC r0, r1, #10
    instruction = decoder->DecodeARM(0xE2C1000A);
    REQUIRE(std::dynamic_pointer_cast<ARM::DataProcessingInstruction>(instruction));
    // SBC r0, r1, #10
    instruction = decoder->DecodeARM(0xE0C1000A);
    REQUIRE(std::dynamic_pointer_cast<ARM::DataProcessingInstruction>(instruction));

    // RSC r0, r1, #10
    instruction = decoder->DecodeARM(0xE2E1000A);
    REQUIRE(std::dynamic_pointer_cast<ARM::DataProcessingInstruction>(instruction));
    // RSC r0, r1, #10
    instruction = decoder->DecodeARM(0xE0E1000A);
    REQUIRE(std::dynamic_pointer_cast<ARM::DataProcessingInstruction>(instruction));

    // TST r0, r1, #10
    instruction = decoder->DecodeARM(0xE311000A);
    REQUIRE(std::dynamic_pointer_cast<ARM::DataProcessingInstruction>(instruction));
    // TST r0, r1, r2
    instruction = decoder->DecodeARM(0xE1110002);
    REQUIRE(std::dynamic_pointer_cast<ARM::DataProcessingInstruction>(instruction));

    // TEQ r0, r1, #10
    instruction = decoder->DecodeARM(0xE331000A);
    REQUIRE(std::dynamic_pointer_cast<ARM::DataProcessingInstruction>(instruction));
    // TEQ r0, r1, r2
    instruction = decoder->DecodeARM(0xE1310002);
    REQUIRE(std::dynamic_pointer_cast<ARM::DataProcessingInstruction>(instruction));

    // CMP r0, r1, #10
    instruction = decoder->DecodeARM(0xE351000A);
    REQUIRE(std::dynamic_pointer_cast<ARM::DataProcessingInstruction>(instruction));
    // CMP r0, r1, r2
    instruction = decoder->DecodeARM(0xE1510002);
    REQUIRE(std::dynamic_pointer_cast<ARM::DataProcessingInstruction>(instruction));

    // CMN r0, r1, #10
    instruction = decoder->DecodeARM(0xE371000A);
    REQUIRE(std::dynamic_pointer_cast<ARM::DataProcessingInstruction>(instruction));
    // CMN r0, r1, r2
    instruction = decoder->DecodeARM(0xE1710002);
    REQUIRE(std::dynamic_pointer_cast<ARM::DataProcessingInstruction>(instruction));

    // ORR r0, r1, #10
    instruction = decoder->DecodeARM(0xE381000A);
    REQUIRE(std::dynamic_pointer_cast<ARM::DataProcessingInstruction>(instruction));
    // ORR r0, r1, r2
    instruction = decoder->DecodeARM(0xE1810002);
    REQUIRE(std::dynamic_pointer_cast<ARM::DataProcessingInstruction>(instruction));

    // MOV r0, #10
    instruction = decoder->DecodeARM(0xE3A0000A);
    REQUIRE(std::dynamic_pointer_cast<ARM::DataProcessingInstruction>(instruction));
    // MOV r0, r2
    instruction = decoder->DecodeARM(0xE1A00002);
    REQUIRE(std::dynamic_pointer_cast<ARM::DataProcessingInstruction>(instruction));

    // BIC r0, r1, #10
    instruction = decoder->DecodeARM(0xE3C1000A);
    REQUIRE(std::dynamic_pointer_cast<ARM::DataProcessingInstruction>(instruction));
    // BIC r0, r1, r2
    instruction = decoder->DecodeARM(0xE1C10002);
    REQUIRE(std::dynamic_pointer_cast<ARM::DataProcessingInstruction>(instruction));

    // MVN r0, #10
    instruction = decoder->DecodeARM(0xE3E0000A);
    REQUIRE(std::dynamic_pointer_cast<ARM::DataProcessingInstruction>(instruction));
    // MVN r0, r2
    instruction = decoder->DecodeARM(0xE1E00002);
    REQUIRE(std::dynamic_pointer_cast<ARM::DataProcessingInstruction>(instruction));

    instruction = decoder->DecodeARM(0xE10F3000);
    REQUIRE(std::dynamic_pointer_cast<ARM::MovePSRToRegisterInstruction>(instruction));

    instruction = decoder->DecodeARM(0xE328F001);
    REQUIRE(std::dynamic_pointer_cast<ARM::MoveRegisterToPSRImmediateInstruction>(instruction));

    instruction = decoder->DecodeARM(0xE128F003);
    REQUIRE(std::dynamic_pointer_cast<ARM::MoveRegisterToPSRRegisterInstruction>(instruction));

    delete decoder;
}
