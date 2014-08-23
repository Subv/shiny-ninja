ShinyNinja
===========
[![Build Status](https://travis-ci.org/Subv/shiny-ninja.svg?branch=master)](https://travis-ci.org/Subv/shiny-ninja)

GBA Emulator written in C++.
Uses Qt5 for the GUI.

ShinyNinja can also be started without a GUI using the "--no-gui" parameter followed by the path to the ROM file you want to load.


TODO List
---------
- [ ] CPU
  * [ ] Implement the rest of the ARM opcodes
    * [x] ARM.3: Branch and Exchange (BX, BLX)
    * [x] ARM.4: Branch and Branch with Link (B, BL, BLX)
    * [x] ARM.5: Data Processing
    * [x] ARM.6: PSR Transfer (MRS, MSR)
    * [x] ARM.7: Multiply and Multiply-Accumulate (MUL,MLA)
    * [x] ARM.9: Single Data Transfer (LDR, STR, PLD)
    * [x] ARM.10: Halfword, Doubleword, and Signed Data Transfer
    * [x] ARM.11: Block Data Transfer (LDM,STM)
    * [ ] ARM.12: Single Data Swap (SWP)
    * [ ] ARM.13: Software Interrupt (SWI)
    * [ ] ARM.14: Coprocessor Data Operations (CDP)
    * [ ] ARM.15: Coprocessor Data Transfers (LDC,STC)
    * [ ] ARM.16: Coprocessor Register Transfers (MRC, MCR)
    * [ ] ARM.17: Undefined Instruction
  * [ ] Implement the rest of the Thumb opcodes
- [ ] GPU
  * [ ] Implement the LCD display routines
- [ ] Video
  * [ ] Implement some kind of graphics adapter class so users can extend it and create their own interfaces
  * [ ] Provide some basic interfaces based on Qt, ncurses, and possibly libcaca
- [ ] MMU
  * [ ] Implement memory writes
  * [ ] Implement read/writes to/from unused memory
- [ ] Miscellaneous
  * [ ] Implement the emulator GUI (Some work has been started, but it's nowhere near finished yet)
  * [ ] Add a JIT compiler (We could use libjit for this)
- [ ] Tests
  * [ ] Add more tests
 

License
---------
To be decided


Contributors
---------
@Subv

@Warpten
