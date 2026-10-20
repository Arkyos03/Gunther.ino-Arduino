/*
 * OBC-STD Arduino core - analogRead() (ADC0/ADC1) and analogWrite() (TCC PWM)
 * Copyright (c) 2026 Damien Fauconnier / AIR ESIEA - MIT License (see LICENSE)
 *
 * analogRead : 0..3.3 V (reference = VDDANA), default resolution 10 bits
 *              like on classic Arduino boards, up to 12 bits.
 * analogWrite: PWM on LED_1, LED_2, BUZZER (732 Hz) and SERVO_x (50 Hz,
 *              TCC1 is shared with the Servo library).
 */
#include "obc_internal.h"

/* ======================================================================
 * ADC
 * ==================================================================== */
static uint8_t s_read_bits  = 10;
static uint8_t s_write_bits = 8;

static void adc_sync(Adc *adc, uint32_t mask)
{
  while (adc->SYNCBUSY.reg & mask) { }
}

static void adc_setup(Adc *adc, uint32_t calib)
{
  adc->CTRLA.reg = ADC_CTRLA_SWRST;
  adc_sync(adc, ADC_SYNCBUSY_SWRST);

  adc->CALIB.reg = (uint16_t)calib;
  adc->CTRLA.reg = ADC_CTRLA_PRESCALER_DIV16;          /* 48 MHz / 16 = 3 MHz ADC clock */
  adc->CTRLB.reg = ADC_CTRLB_RESSEL_12BIT;
  adc_sync(adc, ADC_SYNCBUSY_CTRLB);
  adc->SAMPCTRL.reg = ADC_SAMPCTRL_SAMPLEN(15);        /* long sampling: high impedance sources */
  adc_sync(adc, ADC_SYNCBUSY_SAMPCTRL);
  adc->REFCTRL.reg = ADC_REFCTRL_REFSEL_INTVCC1;       /* reference = VDDANA (3.3 V) */
  adc_sync(adc, ADC_SYNCBUSY_REFCTRL);
  adc->AVGCTRL.reg = ADC_AVGCTRL_SAMPLENUM_1;
  adc_sync(adc, ADC_SYNCBUSY_AVGCTRL);
  adc->INPUTCTRL.reg = ADC_INPUTCTRL_MUXNEG_GND;
  adc_sync(adc, ADC_SYNCBUSY_INPUTCTRL);
  adc->CTRLA.reg |= ADC_CTRLA_ENABLE;
  adc_sync(adc, ADC_SYNCBUSY_ENABLE);
}

void obc_analog_init(void)
{
  MCLK->APBDMASK.reg |= MCLK_APBDMASK_ADC0 | MCLK_APBDMASK_ADC1;
  obc_gclk_enable(ADC0_GCLK_ID, OBC_GCLK_48M);
  obc_gclk_enable(ADC1_GCLK_ID, OBC_GCLK_48M);

  /* Factory calibration values stored in the NVM software calibration area */
  uint32_t sw0 = *((const volatile uint32_t *)NVMCTRL_SW0);
  uint32_t c0 = ADC_CALIB_BIASCOMP((sw0 & ADC0_FUSES_BIASCOMP_Msk) >> ADC0_FUSES_BIASCOMP_Pos) |
                ADC_CALIB_BIASR2R((sw0 & ADC0_FUSES_BIASR2R_Msk) >> ADC0_FUSES_BIASR2R_Pos) |
                ADC_CALIB_BIASREFBUF((sw0 & ADC0_FUSES_BIASREFBUF_Msk) >> ADC0_FUSES_BIASREFBUF_Pos);
  uint32_t c1 = ADC_CALIB_BIASCOMP((sw0 & ADC1_FUSES_BIASCOMP_Msk) >> ADC1_FUSES_BIASCOMP_Pos) |
                ADC_CALIB_BIASR2R((sw0 & ADC1_FUSES_BIASR2R_Msk) >> ADC1_FUSES_BIASR2R_Pos) |
                ADC_CALIB_BIASREFBUF((sw0 & ADC1_FUSES_BIASREFBUF_Msk) >> ADC1_FUSES_BIASREFBUF_Pos);
  adc_setup(ADC0, c0);
  adc_setup(ADC1, c1);
}

void analogReadResolution(int bits)
{
  if (bits < 1)  bits = 1;
  if (bits > 16) bits = 16;
  s_read_bits = (uint8_t)bits;
}

/* Raw 12-bit conversion, -1 if the pin has no analog input */
int obc_analog_read_raw(uint32_t pin)
{
  if (pin >= NUM_DIGITAL_PINS || g_pins[pin].adc == NO_ADC) return -1;
  Adc *adc = (g_pins[pin].adc >> 5) ? ADC1 : ADC0;
  uint32_t ch = g_pins[pin].adc & 0x1Fu;

  obc_pin_set_mux(pin, MUX_B);

  adc->INPUTCTRL.reg = ADC_INPUTCTRL_MUXPOS(ch) | ADC_INPUTCTRL_MUXNEG_GND;
  adc_sync(adc, ADC_SYNCBUSY_INPUTCTRL);

  uint16_t result = 0;
  for (int i = 0; i < 2; i++) {              /* first conversion after a mux change is discarded */
    adc->INTFLAG.reg = ADC_INTFLAG_RESRDY;
    adc->SWTRIG.reg = ADC_SWTRIG_START;
    while ((adc->INTFLAG.reg & ADC_INTFLAG_RESRDY) == 0) { }
    result = adc->RESULT.reg;
  }
  return result;
}

int analogRead(uint32_t pin)
{
  int raw = obc_analog_read_raw(pin);
  if (raw < 0) return 0;
  if (s_read_bits > 12) return raw << (s_read_bits - 12);
  return raw >> (12 - s_read_bits);
}

/* ======================================================================
 * PWM (TCC)
 * ==================================================================== */
static const uint8_t s_tcc_cc_num[5] = { TCC0_CC_NUM, TCC1_CC_NUM, TCC2_CC_NUM, TCC3_CC_NUM, TCC4_CC_NUM };
static bool s_tcc_ready[5];

Tcc *obc_tcc(uint32_t index)
{
  static Tcc *const list[5] = { TCC0, TCC1, TCC2, TCC3, TCC4 };
  return (index < 5) ? list[index] : 0;
}

uint32_t obc_tcc_period(uint32_t index)
{
  return (index == OBC_SERVO_TCC) ? (OBC_SERVO_PERIOD - 1u) : 0xFFFFu;
}

void obc_tcc_init(uint32_t index)
{
  if (index >= 5 || s_tcc_ready[index]) return;
  Tcc *tcc = obc_tcc(index);

  switch (index) {
    case 0: MCLK->APBBMASK.reg |= MCLK_APBBMASK_TCC0; obc_gclk_enable(TCC0_GCLK_ID, OBC_GCLK_48M); break;
    case 1: MCLK->APBBMASK.reg |= MCLK_APBBMASK_TCC1; obc_gclk_enable(TCC1_GCLK_ID, OBC_GCLK_48M); break;
    case 2: MCLK->APBCMASK.reg |= MCLK_APBCMASK_TCC2; obc_gclk_enable(TCC2_GCLK_ID, OBC_GCLK_48M); break;
    case 3: MCLK->APBCMASK.reg |= MCLK_APBCMASK_TCC3; obc_gclk_enable(TCC3_GCLK_ID, OBC_GCLK_48M); break;
    case 4: MCLK->APBDMASK.reg |= MCLK_APBDMASK_TCC4; obc_gclk_enable(TCC4_GCLK_ID, OBC_GCLK_48M); break;
  }

  tcc->CTRLA.reg = TCC_CTRLA_SWRST;
  while (tcc->SYNCBUSY.reg & TCC_SYNCBUSY_SWRST) { }

  /* TCC1 (servos): 48 MHz / 16 = 3 MHz, 20 ms period.  Others: 48 MHz, 16 bits -> 732 Hz */
  uint32_t prescaler = (index == OBC_SERVO_TCC) ? TCC_CTRLA_PRESCALER_DIV16 : TCC_CTRLA_PRESCALER_DIV1;
  tcc->CTRLA.reg = prescaler;
  tcc->WAVE.reg  = TCC_WAVE_WAVEGEN_NPWM;
  while (tcc->SYNCBUSY.reg & TCC_SYNCBUSY_WAVE) { }
  tcc->PER.reg = obc_tcc_period(index);
  while (tcc->SYNCBUSY.reg & TCC_SYNCBUSY_PER) { }
  for (uint32_t i = 0; i < s_tcc_cc_num[index]; i++) {
    tcc->CC[i].reg = 0;
  }
  while (tcc->SYNCBUSY.reg & TCC_SYNCBUSY_CC_Msk) { }

  tcc->CTRLA.reg |= TCC_CTRLA_ENABLE;
  while (tcc->SYNCBUSY.reg & TCC_SYNCBUSY_ENABLE) { }
  s_tcc_ready[index] = true;
}

void obc_tcc_write(uint32_t index, uint32_t cc, uint32_t value)
{
  if (index >= 5 || cc >= s_tcc_cc_num[index]) return;
  obc_tcc_init(index);
  obc_tcc(index)->CCBUF[cc].reg = value;     /* buffered: applied at the next period, no glitch */
}

void analogWriteResolution(int bits)
{
  if (bits < 1)  bits = 1;
  if (bits > 16) bits = 16;
  s_write_bits = (uint8_t)bits;
}

void analogWrite(uint32_t pin, uint32_t value)
{
  if (pin >= NUM_DIGITAL_PINS) return;
  const uint32_t maxval = (1UL << s_write_bits) - 1UL;
  if (value > maxval) value = maxval;

  if (g_pins[pin].pwm == NO_PWM) {
    /* No PWM on this pin: behave like a digital output */
    pinMode(pin, OUTPUT);
    digitalWrite(pin, (value >= (maxval + 1UL) / 2UL) ? HIGH : LOW);
    return;
  }

  uint32_t index = g_pins[pin].pwm >> 4;
  uint32_t wo    = g_pins[pin].pwm & 0x0Fu;
  uint32_t cc    = wo % s_tcc_cc_num[index];
  uint32_t top   = obc_tcc_period(index) + 1UL;
  uint32_t ticks = (uint32_t)(((uint64_t)value * top) / maxval);

  obc_tcc_write(index, cc, ticks);

  PortGroup *g = &PORT->Group[g_pins[pin].port];
  g->DIRSET.reg = 1UL << g_pins[pin].pin;
  obc_pin_set_mux(pin, g_pins[pin].pwm_mux);
}
