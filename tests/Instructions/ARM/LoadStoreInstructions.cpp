#include "catch/catch.hpp"
#include "Common/Instructions/ARM/LoadStoreInstructions.hpp"

TEST_CASE("Load and Store Instructions", "Checks that the load/store instruction structures are working")
{
    ARM::LoadStoreInstruction* load = new ARM::LoadStoreInstruction(0xE59FD0B8);
    
    REQUIRE(load->GetOpcode() == ARM::ARMOpcodes::LDR);
    REQUIRE(load->GetRegister() == 13); // SP
    REQUIRE(load->GetBaseRegister() == 15); // PC
    REQUIRE(load->IsImmediate() == true);
    REQUIRE(load->GetImmediateOffset() == 184);
    delete load;
}