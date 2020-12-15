// SPDX-License-Identifier: LGPL-2.1-only
// Copyright (C) 2020 Marek Kraus <gamelaster@outlook.com>

#ifndef BL602_HAL_MISC_H
#define BL602_HAL_MISC_H
#include <stdint.h>

void setupArduino();

void yield(void);

unsigned long millis(void);
unsigned long micros(void);

void delay(uint32_t ms);
void delayMicroseconds(uint32_t us);

#endif