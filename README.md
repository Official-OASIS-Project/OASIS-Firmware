# OASIS-Firmware
Arduino source code including non standard libraries

## Notes
- Compile the source code with ESP32 Core version 1.06, do no use version 2.X or higher
- You need the additional libraries from the `libraries` folder within the libraries folder of your Arduino sketchbook directory
- The firmware is written to support both the `AD7606-4` (16 bit) and `AD7606C-18` (18 bit) ADC dynamically, based on the set `ADC BIT (0x0C)` value in the EEPROM (see also OASIS-CommandReference). The board schematics however are **not** compatible with the `AD7606C-18` due to small pinout differences.
