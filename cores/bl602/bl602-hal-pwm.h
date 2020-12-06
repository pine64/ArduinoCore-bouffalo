// SPDX-License-Identifier: LGPL-2.1-only
// Copyright (C) 2020 Marek Kraus <gamelaster@outlook.com>

#ifndef BL602_HAL_PWM_H
#define BL602_HAL_PWM_H

#include <stdint.h>

void analogSetFrequency(uint8_t pin, uint32_t frequency);
int8_t analogWrite(uint8_t pin, float val);

#endif