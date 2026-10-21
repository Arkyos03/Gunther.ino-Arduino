/*
 * OBC-STD Minifs - easy functions for the board
 * Copyright (c) 2026 Arkyos03 for AIR ESIEA - MIT License (see LICENSE)
 */
#include "OBC.h"

OBCBoard OBC;

static const uint8_t s_leds[4]     = { LED_1, LED_2, LED_3, LED_4 };
static const uint8_t s_switches[4] = { SWITCH_1, SWITCH_2, SWITCH_3, SWITCH_4 };
static const uint8_t s_jacks[1]    = { JACK_1 };

void OBCBoard::begin()
{
  for (uint8_t i = 0; i < 4; i++) { pinMode(s_leds[i], OUTPUT); digitalWrite(s_leds[i], LOW); }
  for (uint8_t i = 0; i < 4; i++) pinMode(s_switches[i], INPUT_PULLUP);
  pinMode(JACK_1, INPUT_PULLDOWN);          /* 10k pull-down on the board as well */
  pinMode(SD_DETECT, INPUT_PULLUP);
  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, LOW);
}

int OBCBoard::ledPin(uint8_t n)
{
  return (n >= 1 && n <= 4) ? s_leds[n - 1] : -1;
}

void OBCBoard::led(uint8_t n, bool on)
{
  int p = ledPin(n);
  if (p < 0) return;
  pinMode(p, OUTPUT);
  digitalWrite(p, on ? HIGH : LOW);
}

void OBCBoard::ledToggle(uint8_t n)
{
  int p = ledPin(n);
  if (p >= 0) digitalToggle(p);
}

void OBCBoard::ledBrightness(uint8_t n, uint8_t level)
{
  int p = ledPin(n);
  if (p >= 0) analogWrite(p, level);
}

void OBCBoard::allLeds(bool on)
{
  for (uint8_t n = 1; n <= 4; n++) led(n, on);
}

bool OBCBoard::switchClosed(uint8_t n)
{
  if (n < 1 || n > 4) return false;
  return digitalRead(s_switches[n - 1]) == LOW;
}

bool OBCBoard::jackPlugged(uint8_t n)
{
  if (n != 1) return false;
  return digitalRead(s_jacks[n - 1]) == HIGH;      /* jack plugged: pin tied to VCC */
}

bool OBCBoard::liftoff()
{
  /* simple debounce: the jack must stay out for 20 ms */
  if (jackPlugged(1)) return false;
  uint32_t t0 = millis();
  while (millis() - t0 < 20) {
    if (jackPlugged(1)) return false;
  }
  return true;
}

void OBCBoard::beep(uint32_t frequency, uint32_t duration_ms)
{
  tone(BUZZER, frequency, duration_ms);
}

void OBCBoard::beepBlocking(uint32_t frequency, uint32_t duration_ms)
{
  tone(BUZZER, frequency, duration_ms);
  delay(duration_ms);
}

void OBCBoard::beepCount(uint8_t count, uint32_t frequency)
{
  for (uint8_t i = 0; i < count; i++) {
    beepBlocking(frequency, 80);
    delay(120);
  }
}

float OBCBoard::batteryVoltage()
{
  return readBatteryVoltage();
}

bool OBCBoard::sdCardPresent()
{
  return digitalRead(SD_DETECT) == HIGH;
}
