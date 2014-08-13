#include "catch/catch.hpp"
#include "Common/Instructions/ARM/BranchInstruction.hpp"

TEST_CASE("Branch Instructions", "Checks that the branch instruction structures are working")
{
    ARM::BranchInstruction* branch = new ARM::BranchInstruction(0xEA00002E);
    
    REQUIRE(branch->GetOpcode() == ARMOpcodes::B);
    REQUIRE(branch->GetSignedOffset() == 0xC0 - 8); // The 8 is here because the offset is relative to PC, and PC is at <address> + 8 because of the CPU pipeline
    REQUIRE(branch->Link() == false);
    REQUIRE(branch->GetCondition() == InstructionCondition::Always);
}