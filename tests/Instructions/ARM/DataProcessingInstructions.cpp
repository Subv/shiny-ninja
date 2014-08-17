#include "catch/catch.hpp"
#include "Common/Instructions/ARM/DataProcessingInstructions.hpp"
#include "Common/MathHelper.hpp"

TEST_CASE("Data Processing Instructions", "Checks that the data processing instruction structures are working")
{
    auto and = new ARM::DataProcessingInstruction(0xE2004020);

    REQUIRE(and->GetOpcode() == ARM::ARMOpcodes::AND);
    REQUIRE(and->GetFirstOperand() == 0);
    REQUIRE(and->IsImmediate() == true);
    REQUIRE(and->GetDestinationRegister() == 4);
    REQUIRE(and->GetShiftedSecondOperandImmediate() == 32);
    delete and;

    auto mov = new ARM::DataProcessingInstruction(0xE1A00005);
    REQUIRE(mov->GetOpcode() == ARM::ARMOpcodes::MOV);
    REQUIRE(mov->GetFirstOperand() == 0);
    REQUIRE(mov->IsImmediate() == false);
    REQUIRE(mov->GetSecondOperand() == 5);
    delete mov;
}