/*
 * OBC-STD Arduino core - main header
 * Copyright (c) 2026 Arkyos03 for AIR ESIEA - MIT License (see LICENSE)
 *
 * Implements the usual Arduino API (pinMode, digitalWrite, analogRead, delay,
 * millis, Serial ...) for the OBC-STD Minifs board (ATSAMD51J20A).
 */
#ifndef OBC_ARDUINO_H
#define OBC_ARDUINO_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "sam.h"
#include "variant.h"

#define ARDUINO_OBC_CORE_VERSION "0.1.2"

#ifdef __cplusplus
extern "C" {
#endif

/* ---------------------------------------------------------------- types */
typedef bool     boolean;
typedef uint8_t  byte;
typedef uint16_t word;

/* ------------------------------------------------------------ constants */
#define HIGH            1
#define LOW             0

#define INPUT           0x0
#define OUTPUT          0x1
#define INPUT_PULLUP    0x2
#define INPUT_PULLDOWN  0x3

/* attachInterrupt() modes */
#define CHANGE          2
#define FALLING         3
#define RISING          4

#define LSBFIRST        0
#define MSBFIRST        1

#define DEC 10
#define HEX 16
#define OCT 8
#define BIN 2

#define PI          3.1415926535897932384626433832795
#define HALF_PI     1.5707963267948966192313216916398
#define TWO_PI      6.283185307179586476925286766559
#define DEG_TO_RAD  0.017453292519943295769236907684886
#define RAD_TO_DEG  57.295779513082320876798154814105
#define EULER       2.718281828459045235360287471352

#define NOT_A_PIN       0xFF
#define NOT_AN_INTERRUPT (-1)

/* --------------------------------------------------------------- macros */
#define radians(deg)        ((deg) * DEG_TO_RAD)
#define degrees(rad)        ((rad) * RAD_TO_DEG)
#define sq(x)               ((x) * (x))

#define lowByte(w)          ((uint8_t)((w) & 0xff))
#define highByte(w)         ((uint8_t)((w) >> 8))
#define bit(b)              (1UL << (b))
#define bitRead(v, b)       (((v) >> (b)) & 0x01)
#define bitSet(v, b)        ((v) |= (1UL << (b)))
#define bitClear(v, b)      ((v) &= ~(1UL << (b)))
#define bitWrite(v, b, x)   ((x) ? bitSet(v, b) : bitClear(v, b))

#define interrupts()        __enable_irq()
#define noInterrupts()      __disable_irq()

#define clockCyclesPerMicrosecond()   (F_CPU / 1000000UL)
#define clockCyclesToMicroseconds(a)  ((a) / clockCyclesPerMicrosecond())
#define microsecondsToClockCycles(a)  ((a) * clockCyclesPerMicrosecond())

#define digitalPinToInterrupt(p)      ((p) < NUM_DIGITAL_PINS ? (int)(p) : NOT_AN_INTERRUPT)
#define digitalPinIsValid(p)          ((p) < NUM_DIGITAL_PINS)
#define analogInputToDigitalPin(p)    (p)

/* Direct port access (used by some libraries for fast software SPI) */
#define digitalPinToPort(p)           (&(PORT->Group[g_pins[p].port]))
#define digitalPinToBitMask(p)        (1UL << g_pins[p].pin)
#define portOutputRegister(port)      (&((port)->OUT.reg))
#define portInputRegister(port)       (&((port)->IN.reg))
#define portModeRegister(port)        (&((port)->DIR.reg))
#define portSetRegister(port)         (&((port)->OUTSET.reg))
#define portClearRegister(port)       (&((port)->OUTCLR.reg))

#define F(str)          (str)
#define PSTR(str)       (str)
#define PROGMEM
#define pgm_read_byte(addr)   (*(const uint8_t *)(addr))
#define pgm_read_word(addr)   (*(const uint16_t *)(addr))
#define pgm_read_dword(addr)  (*(const uint32_t *)(addr))
#define pgm_read_float(addr)  (*(const float *)(addr))
#define pgm_read_ptr(addr)    (*(void * const *)(addr))
#define memcpy_P              memcpy
#define strlen_P              strlen
#define strcpy_P              strcpy
#define strcmp_P              strcmp

/* ------------------------------------------------------------ prototypes */
void init(void);
void setup(void);
void loop(void);
void yield(void);

void pinMode(uint32_t pin, uint32_t mode);
void digitalWrite(uint32_t pin, uint32_t value);
int  digitalRead(uint32_t pin);
void digitalToggle(uint32_t pin);

int  analogRead(uint32_t pin);
void analogReadResolution(int bits);
void analogWrite(uint32_t pin, uint32_t value);
void analogWriteResolution(int bits);

uint32_t millis(void);
uint32_t micros(void);
void delay(uint32_t ms);
void delayMicroseconds(uint32_t us);

typedef void (*voidFuncPtr)(void);
void attachInterrupt(uint32_t pin, voidFuncPtr callback, uint32_t mode);
void detachInterrupt(uint32_t pin);

void tone(uint32_t pin, uint32_t frequency, uint32_t duration_ms);
void noTone(uint32_t pin);

uint32_t pulseIn(uint32_t pin, uint32_t state, uint32_t timeout_us);
uint32_t pulseInLong(uint32_t pin, uint32_t state, uint32_t timeout_us);
void     shiftOut(uint32_t dataPin, uint32_t clockPin, uint32_t bitOrder, uint8_t val);
uint8_t  shiftIn(uint32_t dataPin, uint32_t clockPin, uint32_t bitOrder);

/* Reboot the board into the UF2 bootloader (same as a double reset) */
void resetToBootloader(void);

/* Board helpers (see also the OBC library) */
float readBatteryVoltage(void);

char *dtostrf(double val, signed char width, unsigned char prec, char *sout);

#ifdef __cplusplus
} /* extern "C" */
#endif

/* ------------------------------------------------------------ C++ part */
#ifdef __cplusplus

/* min / max / constrain / abs that accept mixed types */
template <class T, class L>
static inline auto min(const T &a, const L &b) -> decltype((b < a) ? b : a) { return (b < a) ? b : a; }
template <class T, class L>
static inline auto max(const T &a, const L &b) -> decltype((b < a) ? b : a) { return (a < b) ? b : a; }
template <class T, class L, class H>
static inline auto constrain(const T &x, const L &lo, const H &hi) -> decltype((x < lo) ? lo : ((x > hi) ? hi : x))
{ return (x < lo) ? lo : ((x > hi) ? hi : x); }

#include "WString.h"
#include "Print.h"
#include "Stream.h"
#include "HardwareSerial.h"
#include "USBSerial.h"
#include "Uart.h"

/* Overloads with default arguments */
uint32_t pulseIn(uint32_t pin, uint32_t state);
uint32_t pulseInLong(uint32_t pin, uint32_t state);
void tone(uint32_t pin, uint32_t frequency);

/* WMath */
long random(long howbig);
long random(long howsmall, long howbig);
void randomSeed(unsigned long seed);
long map(long x, long in_min, long in_max, long out_min, long out_max);
float mapFloat(float x, float in_min, float in_max, float out_min, float out_max);

uint16_t makeWord(uint16_t w);
uint16_t makeWord(uint8_t h, uint8_t l);
#define word(...) makeWord(__VA_ARGS__)

#else  /* plain C */

#ifndef min
#define min(a, b) ((a) < (b) ? (a) : (b))
#endif
#ifndef max
#define max(a, b) ((a) > (b) ? (a) : (b))
#endif
#define constrain(x, lo, hi) ((x) < (lo) ? (lo) : ((x) > (hi) ? (hi) : (x)))

#endif /* __cplusplus */

#endif /* OBC_ARDUINO_H */
