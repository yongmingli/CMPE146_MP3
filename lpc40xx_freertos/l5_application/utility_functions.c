#include "FreeRTOS.h"
#include "acceleration.h"
#include "apds.h"
#include "board_io.h"
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
#include "task.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cli_handlers.h"

// function definitaion
bool opensong(sl_string_t song);
bool song_name_ok(sl_string_t song, char song_name[32], char *song_type);

// data type
bool if_mp3(char *type);

// function declaration
bool opensong(sl_string_t song) {
  const char *filename = "file.txt";
  FIL file; // File handle
  UINT bytes_written = 0;
  FRESULT result = f_open(&file, filename, (FA_OPEN_EXISTING));
  if (FR_OK == result)
    return true;
  else
    return false;
}

bool song_name_ok(sl_string_t song, char song_name[32], char *song_type) {

  printf("Song: <%s> ", song); // TESTING:

  int index = 0;
  int temp = 0;
  while (song[index] != '.' && index < 32) {
    song_name[index] = song[index];
    index++;
  }                  // song name read done
  if (index == 32) { // song name longer than 32 or can not find '.'
    printf("Invaild input!!!\n");
    return false;
  } else { // read song type
    index++;
    while (song[index] != ' ') {
      song_type[temp] = song[index];
      temp++;
      index++;
    } // song type read done
    // song_type[temp] = NULL;

    printf("index: <%d> ", index); // TESTING:
    printf("temp: <%d> ", temp);   // TESTING:

    printf("Song: <%s> ", song);            // TESTING:
    printf("Song name: <%s> ", song_name);  // TESTING:
    printf("Song type: <%s> ", &song_type); // TESTING:
    int a = 0;
    int b = 0;
    while (song_name[a] != '\0') {
      printf("Index a: <%d>; Value: <%c> !!!\n", a, song_name);
      a++;
    }
    while (song_type[b] != NULL) {
      printf("Index b: <%d>; Value: <%c> !!!\n", b, song_type);
      b++;
    }

    if (if_mp3(&song_type)) // add || for more data type
      return true;
    else {
      printf("Invaild file type: <%s> !!!\n", song_type);
      return false;
    }
  }
}

bool if_mp3(char *type) {
  if (type[0] == 'm')
    if (type[1] == 'p')
      if (type[2] == '3')
        return true;
      else
        return false;
    else
      return false;
  else
    return false;
}