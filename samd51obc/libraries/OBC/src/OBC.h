/*
 * OBC-STD Minifs - easy functions for the board
 * Copyright (c) 2026 Arkyos03 for AIR ESIEA - MIT License (see LICENSE)
 *
 *   #include <OBC.h>
 *   void setup() { OBC.begin(); }
 *   void loop()  { if (OBC.liftoff()) OBC.led(1, true); }
 *
 * Wiring on the terminal blocks:
 *   - switches are connected between the pin and GND
 *     (internal pull-up: open = HIGH, closed = LOW)
 *   - the launch jack is connected between VCC and the pin
 *     (10k pull-down on the board: plugged = HIGH, removed = LOW)
 *   - LEDs between the pin and GND, with a series resistor
 */
#ifndef OBC_LIB_H
#define OBC_LIB_H

#include <Arduino.h>

class OBCBoard {
public:
  /* Configure every terminal block pin (LED outputs, switch/jack inputs with pull-up) */
  void begin();

  /* LEDs: n = 1..4 */
  void led(uint8_t n, bool on);
  void ledToggle(uint8_t n);
  void ledBrightness(uint8_t n, uint8_t level);   /* 0..255, LED 1 and 2 only */
  void allLeds(bool on);

  /* Switches: n = 1..4, true when the switch is closed (pin to GND) */
  bool switchClosed(uint8_t n);

  /* Jack: n = 1, true when the jack is plugged (pin tied to VCC) */
  bool jackPlugged(uint8_t n);
  /* true as soon as the launch jack (JACK_1) has been pulled out */
  bool liftoff();

  /* Buzzer */
  void beep(uint32_t frequency = 4000, uint32_t duration_ms = 100);
  void beepBlocking(uint32_t frequency = 4000, uint32_t duration_ms = 100);
  void beepCount(uint8_t count, uint32_t frequency = 4000);   /* count short beeps */

  /* Battery voltage in volts (pin PB00, divider 30k/10k) */
  float batteryVoltage();

  /* true when a micro SD card is inserted */
  bool sdCardPresent();

  /* Restart into the bootloader (the board appears as a USB drive) */
  void enterBootloader() { resetToBootloader(); }

private:
  int ledPin(uint8_t n);
};

extern OBCBoard OBC;

#endif
