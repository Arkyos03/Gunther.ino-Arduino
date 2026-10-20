#ifndef BOARD_CONFIG_H
#define BOARD_CONFIG_H

/*
 * OBC-STD Minifs (AIR ESIEA) - configuration for the UF2 bootloader
 * (https://github.com/adafruit/uf2-samdx1, MIT license)
 *
 * Copy this folder to uf2-samdx1/boards/obc_std/ then run:
 *     make BOARD=obc_std
 * Result: build/obc_std/bootloader-obc_std-<version>.bin
 * Copy it here as ../bootloader-obc_std.bin (used by "Burn Bootloader").
 */

#define VENDOR_NAME  "AIR ESIEA"
#define PRODUCT_NAME "OBC-STD Minifs"
#define VOLUME_LABEL "OBCBOOT"
#define INDEX_URL    "https://github.com/"
#define BOARD_ID     "SAMD51J20A-OBC-STD-RevA"

#define CRYSTALLESS  1          /* USB works even if the 32 kHz crystal is missing */

/* pid.codes test PID - replace with your own PID before distribution */
#define USB_VID 0x1209
#define USB_PID 0x0001

#define LED_PIN PIN_PB10        /* LED_1 terminal block: pulses while in bootloader */

#define USE_UART 0

#endif
