// SPDX-License-Identifier: LGPL-2.1-only
// Copyright (C) 2020 Marek Kraus <gamelaster@outlook.com>

#include <bl602-hal-gpio.h>
#include <bl_gpio.h>

void pinMode(uint8_t pin, uint8_t mode)
{
    // TODO: Check if Pin is valid
    if (mode & INPUT) {
        bl_gpio_enable_input(pin, mode & PULLUP ? 1 : 0, mode & PULLDOWN ? 1 : 0);
    } else if (mode & OUTPUT) {
        bl_gpio_enable_output(pin, mode & PULLUP ? 1 : 0, mode & PULLDOWN ? 1 : 0);
    }
}

void digitalWrite(uint8_t pin, uint8_t val)
{
    bl_gpio_output_set(pin, val ? 1 : 0);
}

int digitalRead(uint8_t pin)
{
    int ret, val;
    ret = bl_gpio_input_get(pin, &val);
    if (ret != 0) return LOW;
    return val ? HIGH : LOW;
}
