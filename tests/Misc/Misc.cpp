#include "catch/catch.hpp"
#include "Common/Utilities.hpp"

#include <cstdint>

TEST_CASE("GeneralPurposeRegister", "Executes a batch of tests on the GPRs")
{
    // int z = getchar();
    GeneralPurposeRegister Rd;
    Rd.Value = 0;
    GeneralPurposeRegister Rn;
    Rn.Value = 0xFFFFFFFF;
    GeneralPurposeRegister Rm;
    Rm.Value = 0x00000001;

    Rd = Rn + Rm; // 0xFFFF FFFF + 0x0000 0001 = (0x1 0000 0000) = 0x0000 0000
    REQUIRE(Rd.Carry    == true);  // Carry
    REQUIRE(Rd.Overflow == true);  // Overflow
    REQUIRE(Rd.Borrow   == false); // Doesn't borrow
    REQUIRE(Rd          == 0);

    Rd = Rm - Rn; // 0x0000 0001 - 0xFFFF FFFF = 0xFFFF FFFE = 2
    REQUIRE(Rd.Borrow   == true);
    REQUIRE(Rd.Overflow == false);
    REQUIRE(Rd.Carry    == false);
    REQUIRE(Rd          == 0x00000002);

    // Check for symmetry
    Rd = Rn + Rm;
    REQUIRE(Rd.Carry    == true);
    REQUIRE(Rd.Overflow == true);
    REQUIRE(Rd.Borrow   == false);
    REQUIRE(Rd          == 0);

    Rd = Rn - Rm;
    REQUIRE(Rd.Borrow   == false);
    REQUIRE(Rd.Overflow == false);
    REQUIRE(Rd.Carry    == false);
    REQUIRE(Rd          == 0xFFFFFFFE);

    Rd = 0 - Rm;
    REQUIRE(Rd.Borrow   == false);
    REQUIRE(Rd.Overflow == true);
    REQUIRE(Rd.Carry    == false);
    REQUIRE(Rd          == 0xFFFFFFFF);

}
