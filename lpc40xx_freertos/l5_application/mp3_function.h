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

/* variables */
extern QueueHandle_t song_data;

/* function defination */

// check if song_name is vaild
bool check_name(char song_name[32]);
// check if the file 'song_name' is in SD card
bool check_file(char song_name[32]);
// reading the file 'song_name' from SD card to Q: song_data
bool read_file(char song_name[32]);

bool isMP3(char *file_name);