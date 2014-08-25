#include "catch/catch.hpp"
#include "Common/Instructions/Thumb/DataProcessingInstructions.hpp"
#include "Common/MathHelper.hpp"

TEST_CASE("Thumb Data Processing", "Checks Thumb data processing instructions")
{
    // 010000 0000 001 100 ; 0x400C: AND R4, R1
    auto and = new Thumb::DataProcessingInstruction(0b0100000000001100);
    REQUIRE(and->GetOpcode() == Thumb::ThumbOpcodes::AND);
    REQUIRE(and->IsImmediate() == false);
    REQUIRE(and->GetFirstOperand() == 4);
    REQUIRE(and->GetSecondOperand() == 1);
    delete and;
}
