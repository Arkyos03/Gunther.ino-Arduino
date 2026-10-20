/*
 * OBC-STD - Wire (I2C master) library, Arduino compatible API
 * Copyright (c) 2026 Arkyos03 for AIR ESIEA - MIT License (see LICENSE)
 *
 * Wire : SERCOM5, SDA = PB02, SCL = PB03 (BMP390 at 0x76 + external sensors)
 */
#ifndef OBC_WIRE_H
#define OBC_WIRE_H

#include <Arduino.h>

#define WIRE_BUFFER_SIZE 256
#define BUFFER_LENGTH    WIRE_BUFFER_SIZE
#define WIRE_HAS_END     1
#define WIRE_HAS_TIMEOUT 1

class TwoWire : public Stream {
public:
  TwoWire(uint8_t sercom, uint8_t sda, uint8_t scl, uint8_t mux);

  void begin();
  void begin(uint8_t address) { (void)address; begin(); }   /* slave mode not supported */
  void end();
  void setClock(uint32_t hz);

  /* Timeout of each bus operation (default 25 ms). The bus is reset after a timeout. */
  void setWireTimeout(uint32_t timeout_us = 25000, bool reset_on_timeout = true);
  bool getWireTimeoutFlag() { return _timeoutFlag; }
  void clearWireTimeoutFlag() { _timeoutFlag = false; }

  void beginTransmission(uint8_t address);
  void beginTransmission(int address) { beginTransmission((uint8_t)address); }
  /* 0 = ok, 1 = data too long, 2 = address NACK, 3 = data NACK, 4 = other error, 5 = timeout */
  uint8_t endTransmission(bool sendStop = true);

  size_t requestFrom(uint8_t address, size_t quantity, bool sendStop = true);
  size_t requestFrom(uint8_t address, uint8_t quantity, uint8_t sendStop) { return requestFrom(address, (size_t)quantity, sendStop != 0); }
  size_t requestFrom(int address, int quantity) { return requestFrom((uint8_t)address, (size_t)quantity, true); }
  size_t requestFrom(int address, int quantity, int sendStop) { return requestFrom((uint8_t)address, (size_t)quantity, sendStop != 0); }

  size_t write(uint8_t data) override;
  size_t write(const uint8_t *data, size_t quantity) override;
  using Print::write;
  int available() override { return (int)(_rxLen - _rxPos); }
  int read() override { return (_rxPos < _rxLen) ? _rxBuf[_rxPos++] : -1; }
  int peek() override { return (_rxPos < _rxLen) ? _rxBuf[_rxPos] : -1; }
  void flush() override {}

  void onReceive(void (*)(int)) {}
  void onRequest(void (*)(void)) {}

private:
  bool waitFlag(uint8_t mask);
  void stop();
  void recoverBus();
  void configure();

  uint8_t  _sercom, _sda, _scl, _mux;
  uint32_t _clock;
  uint32_t _timeoutUs;
  bool     _resetOnTimeout;
  bool     _timeoutFlag;
  bool     _started;
  bool     _transmitting;
  uint8_t  _txAddr;
  uint8_t  _txBuf[WIRE_BUFFER_SIZE];
  size_t   _txLen;
  uint8_t  _rxBuf[WIRE_BUFFER_SIZE];
  size_t   _rxLen, _rxPos;
};

extern TwoWire Wire;

#endif
