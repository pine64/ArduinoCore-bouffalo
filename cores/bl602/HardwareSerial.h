// SPDX-License-Identifier: LGPL-2.1-only
// Copyright (C) 2020 Marek Kraus <gamelaster@outlook.com>

#ifndef HardwareSerial_h
#define HardwareSerial_h

#include <inttypes.h>

#include "Stream.h"

// TODO:
#define SERIAL_8N1 0

class HardwareSerial : public Stream
{
public:
    HardwareSerial(uint8_t uart_num) : _uart_num(uart_num) {};
    void begin(unsigned long baud, uint8_t config=SERIAL_8N1);
    void end();
    int available();
    int peek();
    int read();
    size_t read(uint8_t *buffer, size_t size);
    inline size_t read(char * buffer, size_t size)
    {
        return read((uint8_t*) buffer, size);
    }
    int availableForWrite();
    void flush();
    size_t write(uint8_t c);
    size_t write(const uint8_t *buffer, size_t size);
    inline size_t write(const char * buffer, size_t size)
    {
        return write((uint8_t*) buffer, size);
    }

    inline size_t write(unsigned long n)
    {
        return write((uint8_t) n);
    }
    inline size_t write(long n)
    {
        return write((uint8_t) n);
    }
    inline size_t write(unsigned int n)
    {
        return write((uint8_t) n);
    }
    inline size_t write(int n)
    {
        return write((uint8_t) n); 
    }
    using Print::write; // pull in write(str) and write(buf, size) from Print
    operator bool() const { return true; }
protected:
    uint8_t _uart_num;
};

extern HardwareSerial Serial;

extern void serialEventRun(void) __attribute__((weak));

#endif
