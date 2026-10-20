# Third-party components

Everything else in this package is original code under the MIT license (see `LICENSE`).
The components below are included unmodified. Their licenses are permissive: they allow
use, modification and redistribution (also commercial), as long as their copyright
and license texts are kept.

| Component | Folder | License | Copyright |
|---|---|---|---|
| CMSIS Core headers | `cores/obc/cmsis/Core/Include` | Apache-2.0 (`cores/obc/cmsis/LICENSE-CMSIS.txt`) | Arm Limited |
| SAMD51 device headers (DFP) | `cores/obc/cmsis/Device/samd51` | Apache-2.0 (in each file header) | Atmel / Microchip Technology |
| TinyUSB (device stack + CDC class + SAMD driver) | `cores/obc/tinyusb` | MIT (`cores/obc/tinyusb/LICENSE-TinyUSB.txt`) | Ha Thach (tinyusb.org) |

## Tools downloaded by the Arduino IDE (not included in the archive)

| Tool | Used for | License |
|---|---|---|
| arm-none-eabi-gcc (Arm GNU toolchain) | compiling | GPL with GCC runtime library exception: compiled programs are **not** affected |
| bossac (BOSSA) | USB upload | BSD-3-Clause |
| OpenOCD | burning the bootloader with a SWD probe | GPL-2.0 (separate program, does not affect your code) |

## Bootloader

The UF2 bootloader is built from <https://github.com/adafruit/uf2-samdx1> (MIT, Microsoft / Adafruit)
with the board configuration in `bootloaders/obc_std/uf2-board/`.
If you distribute the bootloader binary, keep its MIT license text next to it.
