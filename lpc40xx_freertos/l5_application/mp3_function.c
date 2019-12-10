#include "mp3_function.h"

/***********************************************************************************
 *
 *   private functions:
 *
 ***********************************************************************************/

/***********************************************************************************
 *
 *   END of private functions
 *
 ***********************************************************************************/

/* Function declaration */
bool isMP3(char *file_name) {
  if (file_name[0] == '.') { // check for hidden files (i.e ._FILENAME)
    return false;
  }
  int size = strlen(file_name);
  char *point = strrchr(file_name, '.'); // points to the last occurance of .
  if (point != NULL) {
    return ((strcmp(point, ".mp3") == 0) || (strcmp(point, ".MP3") == 0));
  }
  printf("error\n");
  return false;
}