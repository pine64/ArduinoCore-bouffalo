// SPDX-License-Identifier: LGPL-2.1-only
// Copyright (C) 2020 Marek Kraus <gamelaster@outlook.com>

#ifndef SPI_H
#define SPI_H

#include <stdint.h>
#include <stdlib.h>

#define SPI_HAS_TRANSACTION 1
#define SPI_HAS_NOTUSINGINTERRUPT 0
#define SPI_ATOMIC_VERSION 1

#define SPI_MODE0 0x00
#define SPI_MODE1 0x01
#define SPI_MODE2 0x02
#define SPI_MODE3 0x03

#define SPI_LSBFIRST 0
#define SPI_MSBFIRST 1

#ifndef LSBFIRST
#define LSBFIRST 0
#endif
#ifndef MSBFIRST
#define MSBFIRST 1
#endif

class SPISettings {
public:
  SPISettings() : clock(1000000), bitOrder(SPI_MSBFIRST), dataMode(SPI_MODE0) {}
  SPISettings(uint32_t clock, uint8_t bitOrder, uint8_t dataMode) : clock(clock), bitOrder(bitOrder), dataMode(dataMode) {}
private:
  uint32_t clock;
  uint8_t bitOrder;
  uint8_t dataMode;
  friend class SPIClass;
};

class SPIClass {
public:
  void begin(); // TODO: Pick pins
  void beginTransaction(SPISettings settings);

  uint8_t transfer(uint8_t data);
  uint16_t transfer16(uint16_t data);
  void transfer(void *buf, size_t count);
  void endTransaction(void);

  // Disable the SPI bus
  void end();

  void setBitOrder(uint8_t bitOrder);
  void setDataMode(uint8_t dataMode);
  void setClockDivider(uint8_t clockDiv);

  void setFrequency(uint32_t frequency);

private:
  bool inTransaction = false;
  uint32_t currentFrequency = 0;
  friend void spiInterrupt(void* ctx);
};

extern SPIClass SPI;

#endif