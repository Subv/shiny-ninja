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
