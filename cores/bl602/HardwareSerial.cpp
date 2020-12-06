// SPDX-License-Identifier: LGPL-2.1-only
// Copyright (C) 2020 Marek Kraus <gamelaster@outlook.com>

#include "HardwareSerial.h"
extern "C" {
#include <bl_uart.h>
};

HardwareSerial Serial(0);

void HardwareSerial::begin(unsigned long baud, uint8_t config)
{
    //bl_uart_flush(this->_uart_num); 
    bl_uart_init(_uart_num, 16, 7, 255, 255, baud);
    //bl_uart_int_enable(this->_uart_num);
}

void HardwareSerial::end()
{
    bl_uart_int_disable(_uart_num);
    // TODO: ?
}

int HardwareSerial::available()
{
    return UART_GetRxFifoCount((UART_ID_Type)_uart_num);
}

int HardwareSerial::peek()
{
    // TODO:
    return 0;
}

int HardwareSerial::read()
{
    return bl_uart_data_recv(_uart_num);
}

int HardwareSerial::availableForWrite()
{
    // TODO:
    return 0;
}

void HardwareSerial::flush()
{
    bl_uart_flush(_uart_num);
}

size_t HardwareSerial::write(uint8_t c)
{
    bl_uart_data_send(_uart_num, c);
    return 1;
}

size_t HardwareSerial::write(const uint8_t *buffer, size_t size)
{
    for (size_t i = 0; i < size; i++) {
        bl_uart_data_send(_uart_num, buffer[i]);
    }
    return size;
}