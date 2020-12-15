// SPDX-License-Identifier: LGPL-2.1-only
// Copyright (C) 2020 Marek Kraus <gamelaster@outlook.com>

#include "Arduino.h"

TaskHandle_t rtosMainTask;
extern uint8_t _heap_start;
extern uint8_t _heap_size;
extern uint8_t _heap_wifi_start;
extern uint8_t _heap_wifi_size;
static HeapRegion_t xHeapRegions[] =
{
        { &_heap_start,  (unsigned int) &_heap_size}, //set on runtime
        { &_heap_wifi_start, (unsigned int) &_heap_wifi_size },
        { NULL, 0 }, /* Terminates the array. */
        { NULL, 0 } /* Terminates the array. */
};


extern "C" void __cxa_pure_virtual()
{
  printf("Pure virtual function was called!\n");
  while (1);
}

void eventLoopTask(void *params)
{
  setupArduino();
  setup();
  while (1) {
    loop();
    if (serialEventRun) serialEventRun();
  }
}

extern "C" void bfl_main()
{
  static StackType_t stack[1024 * 20];
  static StaticTask_t eventLoopTaskHandle;
  vPortDefineHeapRegions(xHeapRegions);
  rtosMainTask = xTaskCreateStatic(eventLoopTask, (char*)"eventLoopTask", 1024 * 20, NULL, 15, stack, &eventLoopTaskHandle);
  vTaskStartScheduler();
}