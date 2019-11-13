#include "FreeRTOS.h"
#include "acceleration.h"
#include "apds.h"
#include "app_cli.h"
#include "board_io.h"
#include "cli_handlers.h"
#include "clock.h"
#include "common_macros.h"
#include "delay.h"
#include "diskio.h"
#include "event_groups.h"
#include "ff.h"
#include "gpio.h"
#include "i2c.h"
#include "lpc40xx.h"
#include "lpc_peripherals.h"
#include "queue.h"
#include "semphr.h"
#include "sensors.h"
#include "sj2_cli.h"
#include "task.h"
#include "uart.h"
#include "uart_printf.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// #include "mp3_function.h"
/*
  Lab use
  Global functions and variables
*/

// if: play aaa.mp3
QueueHandle_t instruction; // play      goes there
QueueHandle_t content;     // aaa.mp3   goes there

const char receive_instruction[32];

int main(void) {
  printf("Welcome to our MP3!!!\n");

  instruction = xQueueCreate(1, sizeof(int));
  content = xQueueCreate(1, sizeof(int));

  sj2_cli__init();
  while (true) {
    if (xQueueReceive(instruction, &receive_instruction, portMAX_DELAY)) {
      if (receive_instruction == "play") // highest prity
        mp3_play();
        // else if 
      // more
    }
  }
  // vTaskStartScheduler(); // This function never returns unless RTOS scheduler
  //                        // runs out of memory and fails
  return -1; // return 0;
}
