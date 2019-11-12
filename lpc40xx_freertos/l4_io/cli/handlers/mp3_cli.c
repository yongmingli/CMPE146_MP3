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

QueueHandle_t instruction;
QueueHandle_t content;

const char *cli_instruction;

app_cli_status_e cli__mp3_play(app_cli__argument_t argument,
                               sl_string_t user_input_minus_command_name,
                               app_cli__print_string_function cli_output) {

  instruction = xQueueCreate(1, sizeof(int));
  content = xQueueCreate(1, sizeof(int));

  cli_instruction = "play";
  xQueueSend(instruction, &instruction, portMAX_DELAY);
  /*
  This function does cli like: play The Next Episode.mp3
  */
  sl_string_t cli_content =
      user_input_minus_command_name; // "The Next Episode.mp3" would be here
  xQueueSend(content, &cli_content, portMAX_DELAY);

  return APP_CLI_STATUS__SUCCESS;
}