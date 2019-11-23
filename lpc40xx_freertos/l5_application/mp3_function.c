#include "mp3_function.h"

/***********************************************************************************
 *
 *   private functions:
 *
 ***********************************************************************************/
bool if_mp3(char song_type[8]);

bool if_mp3(char song_type[8]) {
  if (song_type[0] == 'm')
    if (song_type[1] == 'p')
      if (song_type[2] == '3')
        return true;
  return false;
}
/***********************************************************************************
 *
 *   END of private functions
 *
 ***********************************************************************************/

/* Function declaration */
bool check_name(char song_name[32]) {
  int i = 0;
  int temp = 0;
  char song_type[8];
  while (song_name[i] != '.' && i < 32)
    i++;
  if (i == 32)
    return false;
  else {
    i++;
    while (temp < 8 && i < 32) {
      song_type[temp] = song_name[i];
      temp++;
      i++;
    }
    // fprintf(stderr, "temp: %d, song_type: <%s> \n", temp, song_type); //
    // TESTING
    if (if_mp3(song_type))
      return true;
    else {
      fprintf(stderr, "ERROR, file type <%s> is not supported!!!\n",
              song_type); // Error message
      return false;
    }
  }
}

bool check_file(char song_name[32]) {
  FIL file; // File handle
  FRESULT result = f_open(&file, song_name, (FA_OPEN_EXISTING));
  if (FR_OK == result) {
    f_close(&file);
    return true;
  } else
    return false;
}

bool read_file(char song_name[32]) {
  char bytes_512[512];
  FIL file; // File handle
  // UINT *next_read = NULL;
  UINT next_read = 0;
  // fprintf(stderr, "song name 1: <%s>\n", song_name); // TESTING
  FRESULT result = f_open(&file, song_name, (FA_READ | FA_OPEN_EXISTING));

  if (FR_OK == result) {
    // START READING
    do {
      // fprintf(stderr, "song name 2: <%s>\n", song_name); // TESTING
      if (FR_OK == f_read(&file, &bytes_512[0], 512, next_read)) {
        fprintf(stderr, "read data: %c\n", bytes_512[0]); // TESTING
        xQueueSend(song_data, &bytes_512[0], portMAX_DELAY);
      } else
        return false;
    } while (next_read != NULL); // reach the end of the file
    // END READING
    f_close(&file);
    return true;
  } else
    return false;
}