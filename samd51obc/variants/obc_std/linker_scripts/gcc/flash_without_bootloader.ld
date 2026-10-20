/*
 * OBC-STD - linker script for ATSAMD51J20A, no bootloader (flash through SWD)
 * Copyright (c) 2026 Damien Fauconnier / AIR ESIEA - MIT License
 */

MEMORY
{
  FLASH (rx)  : ORIGIN = 0x00000000, LENGTH = 0x00100000
  RAM   (rwx) : ORIGIN = 0x20000000, LENGTH = 0x00040000 - 4
}

INCLUDE obc_sections.ld
