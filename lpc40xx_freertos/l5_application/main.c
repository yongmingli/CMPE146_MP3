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
char song_name[32];
QueueHandle_t song_data;
// QueueHandle_t Q_songname;

// functions
void mp3_init(); // NOT START

void mp3_play(void);     // WORKING
void mp3_cli_play(void); // WORKING
// cli_pause    // DONE
// cli_resume    // DONE
void mp3_vup();  // NOT START
void mp3_vdw();  // NOT START
void mp3_next(); // NOT START
void mp3_last(); // NOT START

int main(void) {
  printf("************************************\n");
  printf("Welcome to our MP3!!!\n");
  printf("************************************\n\n");

  content = xQueueCreate(1, sizeof(char[32]));
  // song_data = xQueueCreate(512, 1);
  song_data = xQueueCreate(512, sizeof(char));
  // Q_songname = xQueueCreate(sizeof(char[32]), 1);

  sj2_cli__init();

  xTaskCreate(mp3_cli_play, "cli_play", 1024U * 8 / (sizeof(void *)), NULL, 3,
              NULL);
  xTaskCreate(mp3_play, "play", 1024U * 32 / (sizeof(void *)), NULL, 2,
              NULL); // This function will always run

  vTaskStartScheduler(); // This function never returns unless RTOS scheduler
                         // runs out of memory and fails
  return -1;             // return 0;
}

void mp3_play(void) {

  char bytes_512[512];
  while (1) {
    /*************************************
     *
     * START PLAYING SONG
     *
     * This funcion will NOT return!
     *
     * It will alway wait for next byte to play!
     *
     *************************************/

    /* TESTING: pause cli and resume cli */
    // TESTING: pause cli and resume cli
    while (1) {
      for (int temp = 0; temp < 999; temp++) {
        printf("temp: %d, wait 3000 tick\n", temp); // TESTING
        vTaskDelay(3000);
      }
    }
    /* TESTING: pause cli and resume cli END */

    // for (int index = 0; index<512; index++){
    //   xQueueReceive(Q_songdata,  &bytes_512[0], portMAX_DELAY);
    //   for (int i = 0; i < sizeof(bytes_512); i++) {
    //     while (!mp3_decoder_needs_data()) {
    //     vTaskDelay(1);
    //   }
    //   spi_send_to_mp3_decoder(bytes_512[i]);
    //   }
    // }
    // }
    /* This funcion will NOT return */
  }
}

void mp3_cli_play(void) {
  while (1) {
    xQueueReceive(content, &song_name, portMAX_DELAY);
    printf("Song name received: %s\n", song_name); // TESTING
    if (check_name(song_name)) {     // check if song_name is vaild
      printf("Song name OK!\n");     // TESTING
      if (check_file(song_name)) {   // song_name is vaild, check if the file
                                     // 'song_name' is in SD card
        printf("file open OK!\n");   // TESTING
        if (read_file(song_name))    // the file 'song_name' is in SD card,
                                     // start reading the file 'song_name' from
                                     // SD card to Q: song_data
          printf("read file OK!\n"); // TESTING
        else
          printf("Error, cannot read file!\n"); // Error message

      } else
        printf("Error, cannot open file!\n"); // Error message
    } else
      printf("Error, check input!\n"); // Error message
  }
}