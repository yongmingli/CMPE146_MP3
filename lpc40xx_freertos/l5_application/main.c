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
char cli_input[32];
QueueHandle_t cli; // play   goes there
QueueHandle_t content; // aaa.mp3   goes there
char song_name[32];
QueueHandle_t song_data;
TaskHandle_t task_handle = xTaskGetHandle(name);
// QueueHandle_t Q_songname;

// functions
void mp3_init(); // NOT START

/* Alway run: the music play function */
void mp3_play(void);     // WORKING
/* */
void mp3_cli_init(void); // WORKING
void mp3_cli_play(void); // WORKING
// cli_pause    // DONE
// cli_resume    // DONE
void mp3_cli_next(void); // NOT START
void mp3_cli_last(void); // NOT START
void mp3_vup();  // NOT START
void mp3_vdw();  // NOT START

int main(void) {
  printf("************************************\n");
  printf("Welcome to our MP3!!!\n");
  printf("************************************\n\n");

  cli = xQueueCreate(1, sizeof(char[32]));
  content = xQueueCreate(1, sizeof(char[32]));
  // song_data = xQueueCreate(512, 1);
  song_data = xQueueCreate(512, sizeof(char));
  // Q_songname = xQueueCreate(sizeof(char[32]), 1);

  sj2_cli__init();

  xTaskCreate(mp3_cli_init, "cli_init", 1024 * 2 ·/ (sizeof(void *)), NULL, 3, NULL);
  xTaskCreate(mp3_play, "play", 1024 * 16 / (sizeof(void *)), NULL, 2, NULL); // This function will always run

  vTaskStartScheduler(); // This function never returns unless RTOS scheduler
                         // runs out of memory and fails
  return -1;             // return 0;
}

void mp3_cli_init(void){
  xQueueReceive(cli, &cli_input[0], portMAX_DELAY);
  if (cli_input == "play"){
    // clear the pervious song play ("cli_play") if there is any
    // get ready for the new incoming cli
    const char name[16] = "cli_play";
    TaskHandle_t task_handle = xTaskGetHandle(name);
    if (NULL == task_handle) {} else vTaskDelete(task_handle); 

    // new cli_play
    xTaskCreate(mp3_cli_play, "cli_play", 1024 * 16 / (sizeof(void *)), NULL, 1, NULL); // This function will always run
  }
  // else if (cli_input == "next")
  // else if (cli_input == "last")
  else printf ("Error, can not identify instruction! Please enter again\n");
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

    // /* TESTING: pause cli and resume cli */
    // while (1) {
    //   for (int temp = 0; temp < 999; temp++) {
    //     printf("temp: %d, wait 3000 tick\n", temp); // TESTING
    //     vTaskDelay(3000);
    //   }
    // }
    // /* TESTING: pause cli and resume cli END */

    xQueueReceive(song_data, &bytes_512[0], portMAX_DELAY);
    for (int i = 0; i < sizeof(bytes_512); i++) {
      /* TESTING */
      printf("Received data: %c\n", bytes_512[0]); // TESTING
      vTaskDelay(1000);                            // TESTING
      /* TESTING */

      //   while (!mp3_decoder_needs_data()) {
      //   vTaskDelay(1);
      // }
      // spi_send_to_mp3_decoder(bytes_512[i]);
    }
  } // END of while(1)
  /* This funcion will NOT return */
}

void mp3_cli_play(void) {
  while (1) {
    xQueueReceive(content, &song_name, portMAX_DELAY);
    // printf("Song name received: %s\n", song_name); // TESTING
    if (check_name(song_name)) { // check if song_name is vaild
      // printf("Song name OK!\n");   // TESTING
      if (check_file(song_name)) { // song_name is vaild, check if the file
                                   // 'song_name' is in SD card
        // printf("file open OK!\n"); // TESTING
        if (read_file(song_name)) {
        } // the file 'song_name' is in SD card,
          // start reading the file 'song_name' from
          // SD card to Q: song_data
        // printf("read file DONE!\n"); // TESTING
        else
          printf("Error, cannot read file!\n"); // Error message
      } else
        printf("Error, cannot open file!\n"); // Error message
    } else
      printf("Error, check input!\n"); // Error message
  }
}