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

#include "mp3_function.h"
/*
  Lab use
  Global functions and variables
*/

// if: play aaa.mp3
QueueHandle_t content; // aaa.mp3   goes there
const char song_name[32];

// QueueHandle_t Q_songname;
// QueueHandle_t Q_songdata;

// functions
void mp3_init(); // NOT START

void mp3_play();  // WORKING
void mp3_pause(); // NOT START
void mp3_vup();   // NOT START
void mp3_vdw();   // NOT START
void mp3_next();  // NOT START
void mp3_last();  // NOT START

int main(void) {
  printf("************************************\n");
  printf("Welcome to our MP3!!!\n");
  printf("************************************\n\n");

  content = xQueueCreate(1, sizeof(char[32]));

  // Q_songname = xQueueCreate(sizeof(char[32]), 1);
  // Q_songdata = xQueueCreate(512, 1);

  sj2_cli__init();

  xTaskCreate(mp3_play, "play", 1024U * 32 / (sizeof(void *)), NULL, 2, NULL);

  vTaskStartScheduler(); // This function never returns unless RTOS scheduler
                         // runs out of memory and fails
  return -1;             // return 0;
}

void mp3_play() {
  while (1) {
    xQueueReceive(content, &song_name, portMAX_DELAY);
    // printf("content received: %s\n", receive_content); // TESTING
    if (check_name(song_name)) {
      // printf("Song name OK!\n"); // TESTING
      if (check_file(song_name)) {
        /*** NO SD, CANNOT TEST NOW!!! ***/
        printf("file open OK!\n"); // TESTING

      } else
        printf("ERROR! CANNNOT OPEN FILE!\n"); // Error message
    } else
      printf("INPUT ERROR!\n"); // Error message
  }
}