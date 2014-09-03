#include "catch/catch.hpp"
#include "Common/Instructions/Thumb/BranchExchangeInstruction.hpp"

TEST_CASE("Branch Instructions Thumb", "Checks that the branch instruction structures are working")
{
    Thumb::LongBranchLinkInstruction* b1 = new Thumb::LongBranchLinkInstruction(0xF7FF);
    int32_t res = 4 + (MathHelper::IntegerSignExtend<11, 32>(b1->GetOffset()) << 12);
    REQUIRE(b1->GetOpcode() == Thumb::ThumbOpcodes::BL);
    REQUIRE(b1->IsTriggeringSubroutineCall() == false);
    delete b1;
    
    Thumb::LongBranchLinkInstruction* b2 = new Thumb::LongBranchLinkInstruction(0xFF81);
    REQUIRE(b2->GetOpcode() == Thumb::ThumbOpcodes::BL);
    REQUIRE(b2->IsTriggeringSubroutineCall() == true);
    res += (b2->GetOffset() & 0x7FF) << 1;
    REQUIRE(res == -0xFA);
    delete b2;
}