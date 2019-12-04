#include "decoder.h"
#include "delay.h"
#include "freertos_interrupt_handlers.h"
#include "gpio.h"
#include "lpc40xx.h"
#include "ssp2_lab.h"

#define VS1053_REG_MODE 0x00
#define VS1053_REG_STATUS 0x01
#define VS1053_REG_BASS 0x02
#define VS1053_REG_CLOCKF 0x03
#define VS1053_REG_DECODETIME 0x04
#define VS1053_REG_AUDATA 0x05
#define VS1053_REG_WRAM 0x06
#define VS1053_REG_WRAMADDR 0x07
#define VS1053_REG_HDAT0 0x08
#define VS1053_REG_HDAT1 0x09
#define VS1053_REG_VOLUME 0x0B

// Function definiations
void decoder_init() {

  // SPI Clock speed, and The speed may be set lower to
  // max_clock_hz
  ssp2__initia(24);

  mp3.rst = gpio__construct_as_output(2, 9);
  mp3.dreq = gpio__construct_as_input(2, 2);
  mp3.xdcs = gpio__construct_as_output(2, 0);
  mp3.xcs = gpio__construct_as_output(2, 7);

  mp3.sck = gpio__construct_with_function(1, 0, 4); // SCK2
  mp3.si = gpio__construct_with_function(1, 1, 4);  // MOSI2
  mp3.so = gpio__construct_with_function(1, 4, 4);  // MISO2

  gpio__set(mp3.xcs);
  gpio__set(mp3.xdcs);
  gpio__set(mp3.rst);

  decoder_hardware_reset();

  decoder_write_reg(VS1053_REG_VOLUME, 0x3838);
  delay__ms(10);
  decoder_write_reg(VS1053_REG_CLOCKF, 0xE000);
  delay__ms(10);
  decoder_write_reg(VS1053_REG_BASS, 0x0806);
  delay__ms(10);
}

void decoder_send_data(uint8_t data[], int size) {
  int counter = 0;
  gpio__reset(mp3.xdcs);
  while (counter < size) {
    // fprintf(stderr, "here0\n");
    while (!gpio__get(mp3.dreq))
      ;
    for (int i = 0; i < 32; i++) {
      ssp2__transfer_byte(data[counter++]);
      // fprintf(stderr, "counter is: <%d>\n", counter);
    }
    // fprintf(stderr, "here1\n");

    while (!gpio__get(mp3.dreq))
      ;
    // fprintf(stderr, "here2\n");
  }
  gpio__set(mp3.xdcs);
  // fprintf(stderr, "here3\n");
}

uint16_t decoder_read_reg(uint8_t address) {
  uint8_t d[2];
  while (!gpio__get(mp3.dreq))
    ;
  gpio__reset(mp3.xcs);
  ssp2__transfer_byte(0x3); // read code
  ssp2__transfer_byte(address);
  d[0] = ssp2__transfer_byte(0x11); // MSB
  while (!gpio__get(mp3.dreq))
    ;
  d[1] = ssp2__transfer_byte(0x11); // LSB
  while (!gpio__get(mp3.dreq))
    ;
  gpio__set(mp3.xcs);
  uint16_t result = d[0] << 8;
  result |= d[1];
  return result;
}

void decoder_write_reg(uint8_t address, uint16_t data) {
  uint8_t h_byte = data >> 8;
  uint8_t l_byte = data;
  while (!gpio__get(mp3.dreq))
    ;
  gpio__reset(mp3.xcs);
  ssp2__transfer_byte(0x02); // write code
  ssp2__transfer_byte(address);
  ssp2__transfer_byte(h_byte); // MSB
  ssp2__transfer_byte(l_byte); // LSB
  gpio__set(mp3.xcs);
  while (!gpio__get(mp3.dreq))
    ;
}

void decoder_hardware_reset() {

  delay__ms(100);
  gpio__reset(mp3.rst);
  delay__ms(100);
  gpio__set(mp3.rst);
  delay__ms(100);
}

void decoder_test() {
  decoder_write_reg(VS1053_REG_MODE, 0x0820);
  uint8_t sine_test[8] = {0x53, 0xEF, 0x6E, 0x7E, 0x00, 0x00, 0x00, 0x00};
  decoder_send_data(sine_test, 8);
  delay__ms(3000);
  uint8_t end_test[8] = {0x45, 0x78, 0x69, 0x74, 0x00, 0x00, 0x00, 0x00};
  decoder_send_data(end_test, 8);
}

void decoder_test2() {
  decoder_write_reg(VS1053_REG_MODE, 0x0820);
  // uint8_t sine_test[8] = {0x53, 0xEF, 0x6E, 0x7E, 0x00, 0x00, 0x00, 0x00};
  decoder_send_data(sound_test, sizeof(sound_test));
  // delay__ms(3000);
  // uint8_t end_test[8] = {0x45, 0x78, 0x69, 0x74, 0x00, 0x00, 0x00, 0x00};
  // decoder_send_data(end_test, 8);
}