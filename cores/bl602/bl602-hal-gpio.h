// SPDX-License-Identifier: LGPL-2.1-only
// Copyright (C) 2020 Marek Kraus <gamelaster@outlook.com>

#ifndef BL602_HAL_GPIO_H
#define BL602_HAL_GPIO_H

#include <stdint.h>

#define LOW               0x0
#define HIGH              0x1

#define INPUT             0x01
#define OUTPUT            0x02
#define PULLUP            0x04
#define INPUT_PULLUP      0x05
#define OUTPUT_PULLUP     0x06
#define PULLDOWN          0x08
#define INPUT_PULLDOWN    0x09
#define OUTPUT_PULLDOWN   0x0A
#define OPEN_DRAIN        0x10
#define OUTPUT_OPEN_DRAIN 0x12

void pinMode(uint8_t pin, uint8_t mode);
void digitalWrite(uint8_t pin, uint8_t val);
int digitalRead(uint8_t pin);

#endif