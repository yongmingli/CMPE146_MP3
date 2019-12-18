//
// CMPE 146
// Project: MP3
// Author: Yongming Li
//
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

#include "gpio_isr.h"
#include "lcd.h"
#include "mp3_function.h"
#include "ssp_lcd.h"
/*
  Lab use
  Global functions and variables
*/

// bottoms
static gpio_s onboard_button2, onboard_button3;
static gpio_s sw_state, sw_up, sw_down, sw_jump, sw_scroll, sw_next, sw_prev,
    sw_play_pause;

// FATFS variables
FATFS fat_fs;
FIL file; // File handle
DIR dir;
FILINFO fno;

// control state variables
bool next = false;
bool pause = true;
bool prev = false;
bool lcd_print = false;
bool jump = false;

// song name buffer
char song_list[20][100]; // 20 song max for now
int song_count = 0;
int current_song = 0;
int jump_index = 0;

// 1: VOL
// 2: BASS
// 3: TREBLE

int state = 1;
void change_state(void);

int vol = 10;                 // default setting for display
uint16_t vol_change = 0x3838; // default setting for change
void vol_down(void);
void vol_up(void);

int bass = 0;
uint16_t BASS = 0x0806; // default setting for change
void bass_down(void);
void bass_up(void);

int treble = 0;
void treble_down(void);
void treble_up(void);

// functions for sws
void change_state(void);    // for sw_state
void state_up(void);        // for sw_up
void state_down(void);      // for sw_down
void play_next(void);       // for sw_next
void play_prev(void);       // for sw_prev
void play_play_pause(void); // for sw_play_pause
void play_jump(void);       // for sw_jump
void scroll_song(void);     // for sw_scroll

void sw_test1(void);
void sw_test2(void);
void sw_test3(void);
void sw_test4(void);
void sw_test5(void);
void sw_test6(void);

// functions
void sd_mount(void);
void mp3_init(void);

// functions for task
void mp3_lcd(void);
void mp3_play(void);

int main(void) {
  /* START UP*/
  {
    /*************************************************************************************
     *
     * buttons setup
     * interrupt assign and enable
     *
     *************************************************************************************/
    onboard_button2 = gpio__construct_as_input(0, 30); // (0, 30);
    onboard_button3 = gpio__construct_as_input(0, 29); // (0, 29);

    sw_state = gpio__construct_as_input(0, 15);     // (0, 15);
    sw_up = gpio__construct_as_input(0, 17);        // (0, 17);
    sw_down = gpio__construct_as_input(0, 30);      // (0, 22);
    sw_next = gpio__construct_as_input(2, 1);       // (2, 1);
    sw_prev = gpio__construct_as_input(2, 4);       // (2, 4);
    sw_play_pause = gpio__construct_as_input(2, 6); // (2, 6);

    gpio0__attach_interrupt(30, GPIO_INTR__RISING_EDGE,
                            play_jump); // onboard_button2
    gpio0__attach_interrupt(29, GPIO_INTR__RISING_EDGE,
                            scroll_song); // onboard_button3

    gpio0__attach_interrupt(15, GPIO_INTR__RISING_EDGE, change_state);
    gpio0__attach_interrupt(17, GPIO_INTR__RISING_EDGE, state_up);
    gpio0__attach_interrupt(22, GPIO_INTR__RISING_EDGE, state_down);
    gpio2__attach_interrupt(1, GPIO_INTR__RISING_EDGE, play_next);
    gpio2__attach_interrupt(4, GPIO_INTR__RISING_EDGE, play_prev);
    gpio2__attach_interrupt(6, GPIO_INTR__RISING_EDGE, play_play_pause);

    // gpio0__attach_interrupt(15, GPIO_INTR__RISING_EDGE, sw_test1);
    // gpio0__attach_interrupt(17, GPIO_INTR__RISING_EDGE, sw_test2);
    // gpio0__attach_interrupt(22, GPIO_INTR__RISING_EDGE, sw_test3);
    // gpio2__attach_interrupt(1, GPIO_INTR__RISING_EDGE, sw_test4);
    // gpio2__attach_interrupt(4, GPIO_INTR__RISING_EDGE, sw_test5);
    // gpio2__attach_interrupt(6, GPIO_INTR__RISING_EDGE, sw_test6);

    lpc_peripheral__enable_interrupt(GPIO_IRQn, gpio0_2__interrupt_dispatcher);
    NVIC_EnableIRQ(GPIO_IRQn);
  }

  mp3_init();

  // lcd_init_ssp();
  // lcd_init();

  // sj2_cli__init(); // For testing only

  xTaskCreate(mp3_lcd, "lcd", 1024U * 1 / (sizeof(void *)), NULL, 2, NULL);
  xTaskCreate(mp3_play, "play", 1024U * 6 / (sizeof(void *)), NULL, 3, NULL);

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
  // lcd_init();
  // printf("MP3 LCD initialized!!!\n");

  printf("MP3 initialization END!!!\n");
  printf("***************************\n\n");
  printf("Welcome to use our MP3!!!\n");
  printf("Play to start playing music!\n");

  printf("HELP: \n");
  printf("1:\n");
  printf("State 1: To change Volume\n");
  printf("State 2: To change Bass\n");
  printf("State 3: To change Treble\n");
  printf("2:\n");
  printf("Press <Scroll> bottom to browse songs, and hit <Jump> bottom to play "
         "the song of your choice.\n");
  printf("If no <Jump> is pressed, <Scroll> would not effect your current "
         "music play!!!\n");
  printf("\n***************************\n\n");

  printf("First Song: %s\n", song_list[current_song]);
  printf("The Song you are viewing: %s\n", song_list[jump_index]);
  printf("Default Volume: %d\n", vol);
  printf("Default Bass: %d\n", bass);
  printf("Default Treble: %d\n", treble);
  printf("Default State: 1\n");
}

void mp3_play(void) {
  char bytes_512[512];
  UINT next_read = 512;
  while (1) {
    lcd_print = true;
    vTaskDelay(100); // for lcd print
    lcd_print = false;

    if (pause)         // START setting
      vTaskDelay(100); // waiting to start
    else {
      // printf("play start\n");                                // TESTING
      // printf("current song: %s\n", song_list[current_song]); // TESTING
      f_open(&file, song_list[current_song], FA_READ);
      while (next_read == 512 && !next && !prev && !jump) {

        taskENTER_CRITICAL(); // DISABLE OTHER FEATURES: interrupt, etc.
        f_read(&file, &bytes_512[0], 512, &next_read);
        taskEXIT_CRITICAL();

        taskENTER_CRITICAL(); // DISABLE OTHER FEATURES: interrupt, etc.
        decoder_send_data(&bytes_512[0], 512);
        taskEXIT_CRITICAL();

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
      } else if (jump) {
        if (jump_index < 0 || jump_index >= song_count)
          printf("ERROR in JUMP!\n");
        else
          current_song = jump_index;
        jump = false;
      } else { // default == next
        if (current_song + 1 < song_count) {
          current_song++;
        } else {
          current_song = 0;
        }
      }
      // printf("next song: %s\n", song_list[current_song]); // TESTING
    }
  }
}

void mp3_lcd(void) {
  while (1) {
    vTaskDelay(10);
    if (lcd_print && !pause) {
      // We do not have a working LCD now!!!
      // So print on Terminal
      printf("Song: %s\n", song_list[current_song]); // TESTING
      lcd_print = false;
    }
  }
}

void sd_mount(void) {
  song_count = 0;
  // mounting sd card
  printf("Start scanning SD card.\n\n");
  f_mount(&fat_fs, "", 1);

  // open root directory
  FRESULT res = f_opendir(&dir, "/");

  // scan for mp3 file
  if (res == FR_OK) {
    for (;;) {
      res = f_readdir(&dir, &fno);
      if (res != FR_OK || fno.fname[0] == 0) {
        printf("\nFound %d songs\n", song_count);
        break; // error or end of loop
      } else if (fno.fattrib & AM_DIR) {
        // a folder, ignore
      } else {
        // save song name if ends with "mp3"
        if (isMP3(fno.fname)) {
          printf("%d: %s\n", song_count, fno.fname);
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

/************************************************************************************
 *
 * Direct from datasheet:
 *    "Thus, maximum volume is 0x0000 and total silence is 0xFEFE."
 *
 * Our default setting is: 0x3838
 *
 * **********************************************************************************/

void vol_down(void) {
  if (vol > 0) {
    vol_change = vol_change + 0x0404;
    vol--;
    if (vol == 0)
      decoder_write_reg(0x0B, 0xFEFE);
    else
      decoder_write_reg(0x0B, vol_change);
  }
  fprintf(stderr, "Volume: %d\n", vol);
}
void vol_up(void) {
  if (vol < 24) {
    vol_change = vol_change - 0x0404;
    vol++;
    decoder_write_reg(0x0B, vol_change);
  }
  fprintf(stderr, "Volume: %d\n", vol);
}
void bass_down(void) {
  if (bass > 0) {
    BASS = BASS - 0x10;
    bass--;
    decoder_write_reg(0x02, BASS);
  }
  fprintf(stderr, "Bass: %d\n", bass);
}
void bass_up(void) {
  if (bass < 15) {
    BASS = BASS + 0x10;
    bass++;
    decoder_write_reg(0x02, BASS);
  }
  fprintf(stderr, "Bass: %d\n", bass);
}
void treble_down(void) {
  if (treble > 0) {
    BASS = BASS - 0x1000;
    treble--;
    decoder_write_reg(0x02, BASS);
  }
  fprintf(stderr, "Treble: %d\n", treble);
}
void treble_up(void) {
  if (treble < 7) {
    BASS = BASS + 0x1000;
    treble++;
    decoder_write_reg(0x02, BASS);
  }
  fprintf(stderr, "Treble: %d\n", treble);
}

/************************************************************************************
 *
 *
 * functions for sws
 *
 * **********************************************************************************/
void change_state(void) {
  if (state == 1) {
    state = 2;
    fprintf(stderr, "State: %d\n", state);
    fprintf(stderr, "You can change BASS now!\n");
  } else if (state == 2) {
    state = 3;
    fprintf(stderr, "State: %d\n", state);
    fprintf(stderr, "You can change TREBLE now!\n");
  } else if (state == 3) {
    state = 1;
    fprintf(stderr, "State: %d\n", state);
    fprintf(stderr, "You can change VOLUME now!\n");
  } else {
    fprintf(stderr, "ERROR!\n");
    fprintf(stderr, "THIS SHOULD NOT PRINT!\n");
  }
  mp3_delay();
}

void state_up(void) {
  if (state == 1)
    vol_up();
  else if (state == 2)
    bass_up();
  else if (state == 3)
    treble_up();
  else {
    fprintf(stderr, "ERROR!\n");
    fprintf(stderr, "THIS SHOULD NOT PRINT!\n");
  }
  mp3_delay();
}

void state_down(void) {
  if (state == 1)
    vol_down();
  else if (state == 2)
    bass_down();
  else if (state == 3)
    treble_down();
  else {
    fprintf(stderr, "ERROR!\n");
    fprintf(stderr, "THIS SHOULD NOT PRINT!\n");
  }
  mp3_delay();
}

void play_next(void) {
  pause = false;
  next = true;
  mp3_delay();
}

void play_prev(void) {
  pause = false;
  prev = true;
  mp3_delay();
}

void play_play_pause(void) {
  if (pause) {
    pause = false;
    fprintf(stderr, "PLAY\n");
  } else {
    pause = true;
    fprintf(stderr, "PAUSE\n");
  }
  mp3_delay();
}

void play_jump(void) {
  pause = false;
  jump = true;
  mp3_delay();
}

void scroll_song(void) {
  jump_index = current_song;
  if (jump_index + 1 < song_count) {
    jump_index++;
  } else {
    jump_index = 0;
  }
  fprintf(stderr, "The Song you are viewing: %s\n", song_list[jump_index]);
  mp3_delay();
}