// SPDX-License-Identifier: LGPL-2.1-only
// Copyright (C) 2020 Marek Kraus <gamelaster@outlook.com>
#include "bl602-hal-pwm.h"
#include <bl602_glb.h>
#include <bl602_pwm.h>
#include <bl_pwm.h>

void analogSetFrequency(uint8_t pin, uint32_t frequency)
{
    uint8_t pwm_channel_id = pin % 5;
    if (frequency < 2000 || frequency > 800000) {
        return;
    }

    bl_pwm_set_freq(pwm_channel_id, frequency);
}

int8_t analogWrite(uint8_t pin, float val)
{
    uint8_t pwm_channel_id = pin % 5;
    if (GLB_GPIO_Get_Fun(pin) != 8) {
        GLB_GPIO_Cfg_Type cfg;

        cfg.drive = 0;
        cfg.smtCtrl = 1;
        cfg.gpioMode = GPIO_MODE_OUTPUT;
        cfg.pullType = GPIO_PULL_DOWN;
        cfg.gpioPin = pin;
        cfg.gpioFun = 8;

        GLB_GPIO_Init(&cfg);
    }

    PWM_Channel_Enable(pwm_channel_id);
    uint16_t period, threshold1, threshold2;
    PWM_Channel_Get(pwm_channel_id, &period, &threshold1, &threshold2);

    threshold1 = 0;
    threshold2 = (uint16_t)(val / 255.0f * period);

    PWM_Channel_Set_Threshold1(pwm_channel_id, threshold1);
    PWM_Channel_Set_Threshold2(pwm_channel_id, threshold2);
}