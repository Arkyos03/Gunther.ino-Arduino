/*
 * OBC-STD Arduino core - internal helpers (not part of the public API)
 * Copyright (c) 2026 Damien Fauconnier / AIR ESIEA - MIT License (see LICENSE)
 */
#ifndef OBC_INTERNAL_H
#define OBC_INTERNAL_H

#include "Arduino.h"

#ifdef __cplusplus
extern "C" {
#endif

/* GCLK generators set up by SystemInit() */
#define OBC_GCLK_CPU      0   /* 120 MHz (DPLL0)          */
#define OBC_GCLK_48M      1   /* 48 MHz  (DFLL48M, USB)   */
#define OBC_GCLK_32K      3   /* 32.768 kHz (XOSC32K or OSCULP32K) */
#define OBC_GCLK_1M       5   /* 1 MHz, DPLL reference    */

/* Route a GCLK generator to a peripheral channel */
static inline void obc_gclk_enable(uint32_t channel, uint32_t generator)
{
  GCLK->PCHCTRL[channel].reg = GCLK_PCHCTRL_GEN(generator) | GCLK_PCHCTRL_CHEN;
  while ((GCLK->PCHCTRL[channel].reg & GCLK_PCHCTRL_CHEN) == 0) { }
}

/* Select a peripheral function (MUX_A..MUX_N) on a pin */
void obc_pin_set_mux(uint32_t pin, uint32_t mux);
/* Same, from raw port/pin numbers */
void obc_port_set_mux(uint32_t port, uint32_t bit, uint32_t mux);
/* Back to plain GPIO (peripheral disconnected) */
void obc_pin_set_gpio(uint32_t pin);

/* SERCOM helpers */
Sercom  *obc_sercom(uint32_t index);
void     obc_sercom_enable_clock(uint32_t index);
uint32_t obc_sercom_irq_base(uint32_t index);   /* first of the 4 IRQ numbers */

/* TCC PWM helpers (wiring_analog.c) */
Tcc     *obc_tcc(uint32_t index);
void     obc_tcc_init(uint32_t index);
uint32_t obc_tcc_period(uint32_t index);
void     obc_tcc_write(uint32_t index, uint32_t cc, uint32_t value);
#define  OBC_SERVO_TCC        1u
#define  OBC_SERVO_TICKS_US   3u       /* TCC1 runs at 3 MHz -> 3 ticks per us  */
#define  OBC_SERVO_PERIOD     60000u   /* 20 ms                                 */

/* USB */
void obc_usb_init(void);
void obc_usb_task(void);

/* Analog init */
void obc_analog_init(void);
int  obc_analog_read_raw(uint32_t pin);   /* 12-bit, -1 if not analog */

#ifdef __cplusplus
}
#endif

#endif
