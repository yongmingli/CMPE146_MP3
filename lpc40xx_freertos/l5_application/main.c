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
QueueHandle_t cli;     // play   goes there
QueueHandle_t content; // aaa.mp3   goes there
char song_name[32];
QueueHandle_t song_data;
char song_list[20][100];
// TaskHandle_t task_handle = xTaskGetHandle(name);
// QueueHandle_t Q_songname;

// functions
void mp3_init(void); // WORKING
/* Alway run: the music play function */
void mp3_play(void); // WORKING
/* */
void mp3_cli_play(void); // WORKING
// cli_pause    // DONE
// cli_resume    // DONE
void mp3_cli_next(void); // NOT START
void mp3_cli_last(void); // NOT START
void mp3_vup();          // NOT START
void mp3_vdw();          // NOT START

void sd_mount(void);

int main(void) {

  // test the decoder
  decoder_init();
  // decoder_test2();
  char bytes_512[512];
  FIL file; // File handle
  // UINT *next_read = NULL;
  UINT next_read = 512;
  char *song_name = "a.mp3";
  // fprintf(stderr, "song name 1: <%s>\n", song_name); // TESTING
  FRESULT result = f_open(&file, song_name, (FA_READ | FA_OPEN_EXISTING));

  if (FR_OK == result) {
    // START READING
    do {
      // fprintf(stderr, "song name 2: <%s>\n", song_name); // TESTING
      if (FR_OK == f_read(&file, &bytes_512[0], 512, &next_read)) {
        decoder_send_data(&bytes_512[0], 512);
      }
    } while (next_read == 512); // reach the end of the file
    // END READING
    f_close(&file);
  }

  // coding
  // printf("************************************\n");
  // printf("Welcome to our MP3!!!\n");
  // printf("************************************\n\n");

  // content = xQueueCreate(1, sizeof(char[32]));

  // Q_songname = xQueueCreate(sizeof(char[32]), 1);
  // Q_songdata = xQueueCreate(512, 1);

  // sj2_cli__init();

  // xTaskCreate(mp3_play, "play", 1024U * 32 / (sizeof(void *)), NULL, 2,
  // NULL);

  vTaskStartScheduler(); // This function never returns unless RTOS scheduler
                         // runs out of memory and fails
  return -1;             // return 0;
}

void mp3_init(void) {
  printf("MP3 initialization START!!!\n");
  /* SD mounting*/
  // sd_mount();

  /* DECODER INIT*/
  decoder_init();
  // Ready the DECODER for music play:
  decoder_write_reg(0x00, 0x0820);
  printf("MP3 decoder initialized!!!\n");

  /* LCD INIT*/
  // printf("MP3 LCD initialized!!!\n");
  printf("MP3 initialization END!!!\n");
}

void mp3_play(void) { // 3
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

    xQueueReceive(song_data, &bytes_512[0], portMAX_DELAY);
    decoder_send_data(&bytes_512[0], 512);

    // /* TESTING: pause cli and resume cli */
    // while (1) {
    //   for (int temp = 0; temp < 999; temp++) {
    //     printf("temp: %d, wait 3000 tick\n", temp); // TESTING
    //     vTaskDelay(3000);
    //   }
    // }
    // /* TESTING: pause cli and resume cli END */

    // for (int i = 0; i < sizeof(bytes_512); i++) {
    //   /* TESTING */
    //   printf("Received data: %c\n", bytes_512[0]); // TESTING
    //   vTaskDelay(1000);                            // TESTING
    //   /* TESTING */
    //   //   while (!mp3_decoder_needs_data()) {
    //   //   vTaskDelay(1);
    //   // }
    //   // spi_send_to_mp3_decoder(bytes_512[i]);
    // }
  } // END of while(1)
  /* This funcion will NOT return */
}

void mp3_cli_play(void) { // 2
  char bytes_512[512];
  FIL file; // File handle
  // UINT *next_read = NULL;
  UINT next_read = 512;
  while (1) {
    xQueueReceive(content, &song_name, portMAX_DELAY);
    // printf("Song name received: %s\n", song_name); // TESTING
    if (check_name(song_name)) { // check if song_name is vaild
      // printf("Song name OK!\n");   // TESTING
      if (check_file(song_name)) { // song_name is vaild, check if the file
                                   // 'song_name' is in SD card
                                   // printf("file open OK!\n"); // TESTING

        // fprintf(stderr, "Here.\n", bytes_512[0]); // TESTING
        // decoder_send_data(sound_test, sizeof(sound_test)); // TESTING

        // // fprintf(stderr, "song name 1: <%s>\n", song_name); // TESTING
        // FRESULT result = f_open(&file, song_name, (FA_READ |
        // FA_OPEN_EXISTING));
        // // fprintf(stderr, "Here.\n", bytes_512[0]); // TESTING

        // if (FR_OK == result) {
        //   // START READING
        //   do {
        //     // fprintf(stderr, "song name 2: <%s>\n", song_name); // TESTING
        //     if (FR_OK == f_read(&file, &bytes_512[0], 512, &next_read)) {
        //       // fprintf(stderr, "read data: %c\n", bytes_512[0]); // TESTING
        //       xQueueSend(song_data, &bytes_512[0], portMAX_DELAY);
        //     } else
        //       printf("Error, cannot read in middle file!\n"); // Error
        //       message
        //   } while (next_read == 512); // reach the end of the file
        //   // END READING
        //   f_close(&file);
        // } else
        //   printf("Error, cannot read file!\n"); // Error message

        // //
        // if (read_file(song_name)) {
        //   // the file 'song_name' is in SD card,
        //   // start reading the file 'song_name' from
        //   // SD card to Q: song_data
        //   // printf("read file DONE!\n"); // TESTING
        // } /*END*/
        // else
        //   printf("Error, cannot read file!\n"); // Error message
        // //
      } else
        printf("Error, cannot open file!\n"); // Error message
    } else
      printf("Error, check input!\n"); // Error message
  }
}

void sd_mount(void) {
  FATFS fat_fs;
  FIL fil;
  DIR dir;
  FILINFO fno;
  int song_count = 0;
  // mounting sd card
  // printf("start mounting\n");
  f_mount(&fat_fs, "", 1);

  // open root directory
  printf("Start scanning SD card.\n");
  FRESULT res = f_opendir(&dir, "/");

  // scan for mp3 file
  if (res == FR_OK) {
    for (;;) {
      res = f_readdir(&dir, &fno);
      if (res != FR_OK || fno.fname[0] == 0) {
        printf("Found %d songs\n\n", song_count);
        break; // error or end of loop
      } else if (fno.fattrib & AM_DIR) {
        // a folder, ignore
      } else {
        // save song name if ends with "mp3"
        if (isMP3(fno.fname)) {
          printf("%s\n", fno.fname);
          strcpy(song_list[song_count++], fno.fname);
        }
        // check for max of buffer
        if (song_count >= 20) {
          printf("Can NOT load more songs.\n");
          break;
        }
      }
    }
  }
  f_closedir(&dir);
  // exit if no song found
  if (song_count == 0)
    exit(1);
  // //remounting sd card
  // printf("start mounting\n");
  // f_mount(&fat_fs,"",1);
}
