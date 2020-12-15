// SPDX-License-Identifier: LGPL-2.1-only
// Copyright (C) 2020 Marek Kraus <gamelaster@outlook.com>

#ifndef WIRE_H
#define WIRE_H

#include <stdint.h>
#include <Stream.h>

#define WIRE_HAS_END 1
#define BUFFER_LENGTH 256

class TwoWire : public Stream
{
protected:
    uint8_t buffer[BUFFER_LENGTH];
    uint8_t bufferPosition;
    uint8_t bufferLength;
    uint8_t transmissionStatus;
    uint8_t transmissionDirection; // 0 - Write, 1 - Reading

    uint16_t slaveAddress;
    void configureI2C(uint8_t transmissionDirection);
public:
    TwoWire() {};
    ~TwoWire() {};

    // bool setPins(uint16_t sda, uint16_t scl);
    // bool begin(uint16_t address);
    bool begin(int16_t sda = -1, int16_t scl = -1, uint32_t frequency = 0);

    void setClock(uint32_t frequency);
    // uint32_t getClock(); // TODO:

    // void setTimeOut(uint16_t timeOutMillis); // This is ESP32 variation, also maybe add setWireTimeout for Arduino AVR compat 
    // uint16_t getTimeOut();

    void beginTransmission(uint16_t address);
    size_t write(uint8_t);
    uint8_t endTransmission(bool stop = true);

    uint8_t requestFrom(uint16_t address, uint8_t quantity);
    uint8_t requestFrom(uint16_t address, uint8_t quantity, uint8_t stop) {
        return this->requestFrom(address, quantity);
        // TODO:
    }
    // uint8_t requestFrom(uint16_t address, uint8_t quantity); // TODO: Subaddress

    size_t write(const uint8_t *, size_t);
    int available();
    int read();
    int peek();
    void flush();
    inline size_t write(unsigned long n) { return write((uint8_t)n); } // This was in Arduino Sources, but I think this is bad
    inline size_t write(long n) { return write((uint8_t)n); }
    inline size_t write(unsigned int n) { return write((uint8_t)n); }
    inline size_t write(int n) { return write((uint8_t)n); }
    using Print::write;
    friend void i2cInterrupt(void* ctx);
};

extern TwoWire Wire;

#endif