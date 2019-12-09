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

// lock variables
// SemaphoreHandle_t decoder_lock;
// SemaphoreHandle_t display_lock;

// FATFS variables
FATFS fat_fs;
FIL file; // File handle
DIR dir;
FILINFO fno;

// control state variables
bool next = false;
// bool pause = false;
bool pause = true;
bool prev = false;
bool lcd_print = false;

// song name buffer
char song_list[20][100]; // 20 song max for now
int song_count = 0;
int current_song = 0;

// functions
void sd_mount(void);
void mp3_init(void);

// functions for task
void mp3_lcd(void);
void mp3_play(void);

int main(void) {
  /* START UP*/
  mp3_init();
  sj2_cli__init(); // For testing only

  /////////////
  // decoder_lock = xSemaphoreCreateMutex();
  // display_lock = xSemaphoreCreateMutex();

  xTaskCreate(mp3_lcd, "lcd", 1024U * 1 / (sizeof(void *)), NULL, 2, NULL);
  xTaskCreate(mp3_play, "play", 1024U * 4 / (sizeof(void *)), NULL, 3, NULL);
  // xTaskCreate(mp3_play, "play", 4096, NULL, 3, NULL);

  vTaskStartScheduler(); // This function never returns unless RTOS scheduler
                         // runs out of memory and fails
  return -1;             // return 0;
}
void mp3_init(void) {
  printf("\n***************************\n");
  printf("MP3 initialization START!!!\n");
  /* SD mounting*/
  sd_mount();

  /* DECODER INIT*/
  decoder_init();
  printf("MP3 decoder initialized!!!\n");

  /* LCD INIT*/
  // printf("MP3 LCD initialized!!!\n");
  printf("MP3 initialization END!!!\n");
  printf("***************************\n\n");
  printf("Welcome to use our MP3!!!\n");
  printf("Play to start playing music!\n\n");
  printf("***************************\n");
}

void mp3_play(void) {
  char bytes_512[512];
  UINT next_read = 512;
  while (1) {
    // xSemaphoreGive(display_lock);
    lcd_print = true;
    vTaskDelay(100); // for lcd print
    lcd_print = false;

    if (pause)
      vTaskDelay(100); // waiting to start
    else {             // START setting (here0 start)
      // while (pause == false) { // START setting (here0 start)
      // printf("play start\n"); // TESTING
      // printf("current song: %s\n", song_list[current_song]); // TESTING
      f_open(&file, song_list[current_song], FA_READ);
      while (next_read == 512 && !next && !prev) {
        f_read(&file, &bytes_512[0], 512, &next_read);
        taskENTER_CRITICAL();
        decoder_send_data(&bytes_512[0], 512);
        taskEXIT_CRITICAL();
        // xSemaphoreGive(decoder_lock);
        while (pause) {
          vTaskDelay(10);
        }
      }
      f_close(&file);
      next_read = 512;
      // printf("play end\n"); // TESTING

      if (prev) {
        if (current_song == 0) {
          current_song = song_count - 1;
        } else {
          current_song--;
        }
        prev = false;
      } else if (next) {
        if (current_song + 1 < song_count) {
          current_song++;
        } else {
          current_song = 0;
        }
        next = false;
      } else { // default == next
        if (current_song + 1 < song_count) {
          current_song++;
        } else {
          current_song = 0;
        }
      }
      // printf("next song: %s\n", song_list[current_song]); // TESTING
      // xSemaphoreGive(display_lock,NULL);
      // vTaskDelay(200);
    } // (here0 end)
  }
}

void mp3_lcd(void) {
  while (1) {
    vTaskDelay(10);
    if (lcd_print && !pause) {
      printf("Song: %s\n", song_list[current_song]);
      lcd_print = false;
    }
    // if (xSemaphoreTake(display_lock, portMAX_DELAY)) {
    //   xSemaphoreGive(display_lock);
    // }
    // while(!xSemaphoreTake(display_lock,100));
  }
}

void sd_mount(void) {
  song_count = 0;
  // mounting sd card
  printf("Start scanning SD card.\n");
  f_mount(&fat_fs, "", 1);

  // open root directory
  FRESULT res = f_opendir(&dir, "/");

  // scan for mp3 file
  if (res == FR_OK) {
    for (;;) {
      res = f_readdir(&dir, &fno);
      if (res != FR_OK || fno.fname[0] == 0) {
        printf("Found %d songs\n", song_count);
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
  if (song_count == 0) {
    printf("NO SONG FOUND!!!.\n");
    exit(1);
  }

  // remounting sd card
  f_mount(&fat_fs, "", 1);
}
