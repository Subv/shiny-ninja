#include "catch/catch.hpp"
/*#include "GPU/GPU.hpp"

TEST_CASE("GPU", "Executes a batch of tests on the GPU")
{
    sGPU->WriteUInt16(0x04000000, 0x8000); // Write some stuff in DISPCNT
    REQUIRE(sGPU->ReadUInt16(0x04000000) == 0x8000);

    REQUIRE(sGPU->DISPCNT().Bits->BG0 == 1);

    // Update bg mode - does it mirror in memory ?
    sGPU->DISPCNT().Bits->BgMode = 2;
    REQUIRE(sGPU->ReadUInt16(0x04000000) == 0x8002);
}
*/
