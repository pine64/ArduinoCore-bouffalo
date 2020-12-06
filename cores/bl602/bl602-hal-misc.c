// SPDX-License-Identifier: LGPL-2.1-only
// Copyright (C) 2020 Marek Kraus <gamelaster@outlook.com>

#include "bl602-hal-misc.h"
#include <FreeRTOS.h>
#include <task.h>
#include <bl_timer.h>
#include <bl_pwm.h>

void setupArduino()
{
    for (int i = 0; i < 5; i++) {
        PWM_CH_CFG_Type pwm_cfg = {
            .ch = i,
            .clk = PWM_CLK_XCLK,        //PWM_CLK_BCLK PWM_CLK_XCLK
            .stopMode = PWM_STOP_ABRUPT,//PWM_STOP_ABRUPT:default PWM_STOP_GRACEFUL:no change 
            .pol = PWM_POL_NORMAL,      //first low
            .clkDiv = 0,                //40/2 = 20M
            .period = BL_PWM_CLK / 60000,
            .threshold1 = 0,
            .threshold2 = 0,
            .intPulseCnt = 0,
        };


        PWM_Channel_Disable(i);
        PWM_Channel_Init(&pwm_cfg);
    }
}

void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize)
{
    /* If the buffers to be provided to the Idle task are declared inside this
    function then they must be declared static - otherwise they will be allocated on
    the stack and so not exists after this function exits. */
    static StaticTask_t xIdleTaskTCB;
    static StackType_t uxIdleTaskStack[ configMINIMAL_STACK_SIZE ];

    /* Pass out a pointer to the StaticTask_t structure in which the Idle task's
    state will be stored. */
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

    /* Pass out the array that will be used as the Idle task's stack. */
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;

    /* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
    Note that, as the array is necessarily of type StackType_t,
    configMINIMAL_STACK_SIZE is specified in words, not bytes. */
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

/* configSUPPORT_STATIC_ALLOCATION and configUSE_TIMERS are both set to 1, so the
application must provide an implementation of vApplicationGetTimerTaskMemory()
to provide the memory that is used by the Timer service task. */
void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize)
{
    /* If the buffers to be provided to the Timer task are declared inside this
    function then they must be declared static - otherwise they will be allocated on
    the stack and so not exists after this function exits. */
    static StaticTask_t xTimerTaskTCB;
    static StackType_t uxTimerTaskStack[ configTIMER_TASK_STACK_DEPTH ];

    /* Pass out a pointer to the StaticTask_t structure in which the Timer
    task's state will be stored. */
    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;

    /* Pass out the array that will be used as the Timer task's stack. */
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;

    /* Pass out the size of the array pointed to by *ppxTimerTaskStackBuffer.
    Note that, as the array is necessarily of type StackType_t,
    configTIMER_TASK_STACK_DEPTH is specified in words, not bytes. */
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName )
{
    puts("Stack Overflow checked\r\n");
    while (1) {
        /*empty here*/
    }
}

void vApplicationMallocFailedHook(void)
{
    printf("Memory Allocate Failed. Current left size is %d bytes\r\n",
        xPortGetFreeHeapSize()
    );
    while (1) {
        /*empty here*/
    }
}

void vApplicationIdleHook(void)
{
    __asm volatile(
            "   wfi     "
    );
    /*empty*/
}

void vAssertCalled(void)
{
    volatile uint32_t ulSetTo1ToExitFunction = 0;

    taskDISABLE_INTERRUPTS();
    while( ulSetTo1ToExitFunction != 1 ) {
        __asm volatile( "NOP" );
    }
}


unsigned long millis(void)
{
  return (xTaskGetTickCount() * 1000) / configTICK_RATE_HZ;
}

unsigned long micros(void)
{
    return bl_timer_now_us();
}

void delay(uint32_t ms)
{
    vTaskDelay(ms / portTICK_PERIOD_MS);
}

void delayMicroseconds(uint32_t us)
{
    bl_timer_delay_us(us);
}