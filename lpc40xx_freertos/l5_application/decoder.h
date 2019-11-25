#include "delay.h"
#include "freertos_interrupt_handlers.h"
#include "gpio.h"
#include "lpc40xx.h"
#include "ssp2_lab.h"
#include <stdint.h>
#include <stdio.h>

// Variables
typedef struct {
  gpio_s rst;
  gpio_s dreq;
  gpio_s xdcs;
  gpio_s xcs;

  gpio_s sck;
  gpio_s so;
  gpio_s si;
} decoder_pin;

decoder_pin mp3;

// Function declareations
void decoder_init();
void decoder_test();
void decoder_send_data(uint8_t data[], int size);
uint16_t decoder_read_reg(uint8_t address);
void decoder_write_reg(uint8_t address, uint16_t data);
void decoder_hardware_reset();