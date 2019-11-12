#include "mp3_function.h"

// function declaration
void mp3_play() {
  xQueueReceive(content, &receive_content, portMAX_DELAY);
  printf("receive: <%s>", receive_content);
}