# OASIS-Firmware
Arduino source code including non standard libraries

[![Codacy Badge](https://api.codacy.com/project/badge/Grade/87f7ffccd85d4116b4e7a6fd2ee3fbe4)](https://app.codacy.com/gh/Official-OASIS-Project/OASIS-Firmware?utm_source=github.com&utm_medium=referral&utm_content=Official-OASIS-Project/OASIS-Firmware&utm_campaign=Badge_Grade)

## Notes
- Compile the source code with ESP32 Core version 1.06, do no use version 2.X or higher
- You need the additional libraries from the `libraries` folder within the libraries folder of your Arduino sketchbook directory
- The firmware is written to support both the `AD7606-4` (16 bit) and `AD7606C-18` (18 bit) ADC dynamically, based on the set `ADC BIT (0x0C)` value in the EEPROM (see also OASIS-CommandReference). The board schematics however are **not** compatible with the `AD7606C-18` due to small pinout differences.
