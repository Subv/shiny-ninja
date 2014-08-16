#include "catch/catch.hpp"
#include "Common/Instructions/ARM/BranchInstructions.hpp"

TEST_CASE("Branch Instructions", "Checks that the branch instruction structures are working")
{
    ARM::BranchInstruction* b = new ARM::BranchInstruction(0xEA00002E);
    
    REQUIRE(b->GetOpcode() == ARM::ARMOpcodes::B);
    REQUIRE(b->GetSignedOffset() == 0xC0 - 8); // The 8 is here because the offset is relative to PC, and PC is at <address> + 8 because of the CPU pipeline
    REQUIRE(b->Link() == false);
    REQUIRE(b->GetCondition() == InstructionCondition::Always);
    delete b;

    auto bx = new ARM::BranchLinkExchangeRegisterInstruction(0xE12FFF10);

    REQUIRE(bx->GetOpcode() == ARM::ARMOpcodes::BX);
    REQUIRE(bx->GetCondition() == InstructionCondition::Always);
    REQUIRE(bx->GetRegister() == 0);
    REQUIRE(bx->Link() == false);
    delete bx;
}