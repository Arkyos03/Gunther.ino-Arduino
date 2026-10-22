#ifndef BOARD_CONFIG_H
#define BOARD_CONFIG_H

/*
 * Gunther.ino (AIR ESIEA) - configuration for the UF2 bootloader
 * (https://github.com/adafruit/uf2-samdx1, MIT license)
 *
 * Copy this folder to uf2-samdx1/boards/gunther/ then run:
 *     make BOARD=gunther
 * Result: build/gunther/bootloader-gunther-<version>.bin
 * Copy it here as ../bootloader-gunther.bin (used by "Burn Bootloader").
 */

#define VENDOR_NAME  "AIR ESIEA"
#define PRODUCT_NAME "Gunther.ino"
#define VOLUME_LABEL "GUNTHERBOOT"
#define INDEX_URL    "https://github.com/"
#define BOARD_ID     "SAMD51J20A-Gunther-RevA"

#define CRYSTALLESS  1          /* USB works even if the 32 kHz crystal is missing */

/* pid.codes test PID - replace with your own PID before distribution */
#define USB_VID 0x1209
#define USB_PID 0x0001

#define LED_PIN PIN_PB10        /* LED_1 terminal block: pulses while in bootloader */

/* Built-in RGB LED SK6805-EC15 on PA08 (through a 74AHCT1G125 buffer):
 * red at start-up, green when the USB is connected (bootloader ready) */
#define BOARD_NEOPIXEL_PIN   PIN_PA08
#define BOARD_NEOPIXEL_COUNT 1

#define USE_UART 0

#endif
