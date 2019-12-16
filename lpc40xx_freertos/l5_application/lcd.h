//
// CMPE 146
// Project: MP3
// Author: Yongming Li
//
#include "FreeRTOS.h"
#include "clock.h"
#include "delay.h"
#include "gpio.h"
#include "lpc40xx.h"
#include "lpc_peripherals.h"
#include "task.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

gpio_s E;
gpio_s RS;
gpio_s DB0;
gpio_s DB1;
gpio_s DB2;
gpio_s DB3;
gpio_s DB4;
gpio_s DB5;
gpio_s DB6;
gpio_s DB7;

void lcd_init(void);
void display_on(void);
void display_clear(void);
void display(char *input);
void set_2_lines(void);
void next_line(void);
void first_line(void);