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

/* variables */

/* function defination */

// check if song_name is vaild
bool isMP3(char *file_name);
void mp3_delay(void);