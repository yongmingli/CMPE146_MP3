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
  UINT bytes_written = 0;
  FRESULT result = f_open(&file, song_name, (FA_READ | FA_OPEN_EXISTING));

  if (FR_OK == result) {
    // START READING
    // int index = 0;
    // while (!file.end()) {
    //   if (FR_OK == f_read(&file, bytes_512[index], sizeof(bytes_512[index]),
    //                       &bytes_written)) {
    //     xQueueSend(song_data, &bytes_512[index], portMAX_DELAY);
    //     index++;
    //   } else
    //     return false;
    // }
    // END READING
    f_close(&file);
    return true;
  } else
    return false;
}