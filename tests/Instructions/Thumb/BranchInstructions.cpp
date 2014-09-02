#include "catch/catch.hpp"
#include "Common/Instructions/Thumb/BranchExchangeInstruction.hpp"

TEST_CASE("Branch Instructions Thumb", "Checks that the branch instruction structures are working")
{
    Thumb::LongBranchLinkInstruction* b1 = new Thumb::LongBranchLinkInstruction(0xF7FF);
    REQUIRE(b1->GetOpcode() == Thumb::ThumbOpcodes::BL);
    REQUIRE(b1->IsTriggeringSubroutineCall() == false);
    delete b1;

    Thumb::LongBranchLinkInstruction* b2 = new Thumb::LongBranchLinkInstruction(0xFF81);
    REQUIRE(b2->GetOpcode() == Thumb::ThumbOpcodes::BL);
    REQUIRE(b1->IsTriggeringSubroutineCall() == true);
    delete b2;

    
}