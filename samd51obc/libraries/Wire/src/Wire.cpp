/*
 * OBC-STD - Wire (I2C master) library
 * Copyright (c) 2026 Arkyos03 for AIR ESIEA - MIT License (see LICENSE)
 */
#include "Wire.h"
#include "obc_internal.h"

TwoWire Wire(5, PIN_WIRE_SDA, PIN_WIRE_SCL, MUX_D);

#define I2C_RISE_TIME_NS 125u

TwoWire::TwoWire(uint8_t sercom, uint8_t sda, uint8_t scl, uint8_t mux)
  : _sercom(sercom), _sda(sda), _scl(scl), _mux(mux), _clock(100000UL),
    _timeoutUs(25000UL), _resetOnTimeout(true), _timeoutFlag(false), _started(false),
    _transmitting(false), _txAddr(0), _txLen(0), _rxLen(0), _rxPos(0)
{
}

static inline void i2c_sync(Sercom *s)
{
  while (s->I2CM.SYNCBUSY.reg & SERCOM_I2CM_SYNCBUSY_SYSOP) { }
}

void TwoWire::configure()
{
  Sercom *s = obc_sercom(_sercom);
  obc_sercom_enable_clock(_sercom);

  s->I2CM.CTRLA.reg = SERCOM_I2CM_CTRLA_SWRST;
  while (s->I2CM.SYNCBUSY.reg & SERCOM_I2CM_SYNCBUSY_SWRST) { }

  s->I2CM.CTRLA.reg = SERCOM_I2CM_CTRLA_MODE(5) |          /* I2C master                 */
                      SERCOM_I2CM_CTRLA_SDAHOLD(2) |       /* 300-600 ns SDA hold time   */
                      SERCOM_I2CM_CTRLA_INACTOUT(3) |      /* release a stuck bus        */
                      SERCOM_I2CM_CTRLA_SPEED(_clock > 400000UL ? 1 : 0);
  s->I2CM.CTRLB.reg = 0;
  i2c_sync(s);

  /* fSCL = fGCLK / (10 + 2 * BAUD + fGCLK * Trise) */
  uint32_t f = VARIANT_GCLK1_FREQ;
  int32_t baud = ((int32_t)(f / _clock) - 10 - (int32_t)((uint64_t)f * I2C_RISE_TIME_NS / 1000000000ULL)) / 2;
  if (baud < 1)   baud = 1;
  if (baud > 255) baud = 255;
  s->I2CM.BAUD.reg = SERCOM_I2CM_BAUD_BAUD((uint32_t)baud);

  s->I2CM.CTRLA.reg |= SERCOM_I2CM_CTRLA_ENABLE;
  while (s->I2CM.SYNCBUSY.reg & SERCOM_I2CM_SYNCBUSY_ENABLE) { }

  s->I2CM.STATUS.reg = SERCOM_I2CM_STATUS_BUSSTATE(1);      /* force IDLE state */
  i2c_sync(s);

  obc_pin_set_mux(_sda, _mux);
  obc_pin_set_mux(_scl, _mux);
}

void TwoWire::begin()
{
  configure();
  _started = true;
}

void TwoWire::end()
{
  if (!_started) return;
  Sercom *s = obc_sercom(_sercom);
  s->I2CM.CTRLA.reg &= ~SERCOM_I2CM_CTRLA_ENABLE;
  while (s->I2CM.SYNCBUSY.reg & SERCOM_I2CM_SYNCBUSY_ENABLE) { }
  obc_pin_set_gpio(_sda);
  obc_pin_set_gpio(_scl);
  _started = false;
}

void TwoWire::setClock(uint32_t hz)
{
  if (hz < 10000UL)   hz = 10000UL;
  if (hz > 1000000UL) hz = 1000000UL;
  _clock = hz;
  if (_started) configure();
}

void TwoWire::setWireTimeout(uint32_t timeout_us, bool reset_on_timeout)
{
  _timeoutUs = timeout_us;
  _resetOnTimeout = reset_on_timeout;
}

/* Free a slave that keeps SDA low (e.g. after a reset in the middle of a transfer) */
void TwoWire::recoverBus()
{
  Sercom *s = obc_sercom(_sercom);
  s->I2CM.CTRLA.reg &= ~SERCOM_I2CM_CTRLA_ENABLE;
  while (s->I2CM.SYNCBUSY.reg & SERCOM_I2CM_SYNCBUSY_ENABLE) { }

  obc_pin_set_gpio(_sda);
  obc_pin_set_gpio(_scl);
  pinMode(_sda, INPUT_PULLUP);
  pinMode(_scl, OUTPUT);
  for (int i = 0; i < 9 && digitalRead(_sda) == LOW; i++) {
    digitalWrite(_scl, LOW);  delayMicroseconds(5);
    digitalWrite(_scl, HIGH); delayMicroseconds(5);
  }
  pinMode(_scl, INPUT_PULLUP);
  configure();
}

bool TwoWire::waitFlag(uint8_t mask)
{
  Sercom *s = obc_sercom(_sercom);
  uint32_t start = micros();
  while ((s->I2CM.INTFLAG.reg & mask) == 0) {
    if (_timeoutUs && (micros() - start) > _timeoutUs) {
      _timeoutFlag = true;
      if (_resetOnTimeout) recoverBus();
      return false;
    }
  }
  return true;
}

void TwoWire::stop()
{
  Sercom *s = obc_sercom(_sercom);
  s->I2CM.CTRLB.reg = (s->I2CM.CTRLB.reg & ~SERCOM_I2CM_CTRLB_CMD_Msk) | SERCOM_I2CM_CTRLB_CMD(3);
  i2c_sync(s);
}

void TwoWire::beginTransmission(uint8_t address)
{
  _txAddr = address;
  _txLen = 0;
  _transmitting = true;
}

size_t TwoWire::write(uint8_t data)
{
  if (!_transmitting || _txLen >= WIRE_BUFFER_SIZE) return 0;
  _txBuf[_txLen++] = data;
  return 1;
}

size_t TwoWire::write(const uint8_t *data, size_t quantity)
{
  size_t n = 0;
  while (n < quantity && write(data[n])) n++;
  return n;
}

uint8_t TwoWire::endTransmission(bool sendStop)
{
  if (!_transmitting) return 4;
  _transmitting = false;
  if (!_started) begin();
  Sercom *s = obc_sercom(_sercom);

  s->I2CM.ADDR.reg = SERCOM_I2CM_ADDR_ADDR((uint32_t)_txAddr << 1);
  i2c_sync(s);
  if (!waitFlag(SERCOM_I2CM_INTFLAG_MB)) return 5;
  if (s->I2CM.STATUS.reg & (SERCOM_I2CM_STATUS_ARBLOST | SERCOM_I2CM_STATUS_BUSERR)) return 4;
  if (s->I2CM.STATUS.reg & SERCOM_I2CM_STATUS_RXNACK) { stop(); return 2; }

  for (size_t i = 0; i < _txLen; i++) {
    s->I2CM.DATA.reg = _txBuf[i];
    i2c_sync(s);
    if (!waitFlag(SERCOM_I2CM_INTFLAG_MB)) return 5;
    if (s->I2CM.STATUS.reg & SERCOM_I2CM_STATUS_RXNACK) { stop(); return 3; }
  }
  if (sendStop) stop();
  return 0;
}

size_t TwoWire::requestFrom(uint8_t address, size_t quantity, bool sendStop)
{
  (void)sendStop;                /* a read always ends with NACK + STOP */
  _rxLen = _rxPos = 0;
  if (quantity == 0) return 0;
  if (quantity > WIRE_BUFFER_SIZE) quantity = WIRE_BUFFER_SIZE;
  if (!_started) begin();
  Sercom *s = obc_sercom(_sercom);

  s->I2CM.CTRLB.reg &= ~SERCOM_I2CM_CTRLB_ACKACT;           /* ACK the received bytes */
  i2c_sync(s);
  s->I2CM.ADDR.reg = SERCOM_I2CM_ADDR_ADDR(((uint32_t)address << 1) | 1u);
  i2c_sync(s);

  for (size_t i = 0; i < quantity; i++) {
    if (!waitFlag(SERCOM_I2CM_INTFLAG_SB | SERCOM_I2CM_INTFLAG_MB)) return 0;
    if (s->I2CM.INTFLAG.reg & SERCOM_I2CM_INTFLAG_MB) {      /* address NACK or bus error */
      stop();
      return 0;
    }
    uint8_t value = (uint8_t)s->I2CM.DATA.reg;
    i2c_sync(s);
    _rxBuf[_rxLen++] = value;

    uint32_t ctrlb = s->I2CM.CTRLB.reg & ~(SERCOM_I2CM_CTRLB_CMD_Msk | SERCOM_I2CM_CTRLB_ACKACT);
    if (i + 1 < quantity) ctrlb |= SERCOM_I2CM_CTRLB_CMD(2);                           /* ACK, read next */
    else                  ctrlb |= SERCOM_I2CM_CTRLB_ACKACT | SERCOM_I2CM_CTRLB_CMD(3); /* NACK, STOP     */
    s->I2CM.CTRLB.reg = ctrlb;
    i2c_sync(s);
  }
  return _rxLen;
}
