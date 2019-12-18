//
// CMPE 146
// Project: MP3
// Author: Yongming Li
//
#include "ssp_lcd.h"
#include "delay.h"
#include "freertos_interrupt_handlers.h"
#include "gpio.h"
#include "lcd_ssp.h"
#include "lpc40xx.h"

/***********************************************************************************
 *
 *   private functions:
 *
 ***********************************************************************************/

void lcd_reg_init(void);
void lcd_reg_init(void) {
  lcd_write_command(0xae); //--turn off oled panel

  lcd_write_command(0x15); // set column address
  lcd_write_command(0x00); // start column   0
  lcd_write_command(0x7f); // end column   127

  lcd_write_command(0x75); // set row address
  lcd_write_command(0x00); // start row   0
  lcd_write_command(0x7f); // end row   127

  lcd_write_command(0x81); // set contrast control
  lcd_write_command(0x80);

  lcd_write_command(0xa0); // gment remap
  lcd_write_command(0x51); // 51

  lcd_write_command(0xa1); // start line
  lcd_write_command(0x00);

  lcd_write_command(0xa2); // display offset
  lcd_write_command(0x00);

  lcd_write_command(0xa4); // rmal display
  lcd_write_command(0xa8); // set multiplex ratio
  lcd_write_command(0x7f);

  lcd_write_command(0xb1); // set phase leghth
  lcd_write_command(0xf1);

  lcd_write_command(0xb3); // set dclk
  lcd_write_command(0x00); // 80Hz:0xc1 90Hz:0xe1   100Hz:0x00   110Hz:0x30
                           // 120Hz:0x50   130Hz:0x70     01

  lcd_write_command(0xab); //
  lcd_write_command(0x01); //

  lcd_write_command(0xb6); // set phase leghth
  lcd_write_command(0x0f);

  lcd_write_command(0xbe);
  lcd_write_command(0x0f);

  lcd_write_command(0xbc);
  lcd_write_command(0x08);

  lcd_write_command(0xd5);
  lcd_write_command(0x62);

  lcd_write_command(0xfd);
  lcd_write_command(0x12);
}

/***********************************************************************************
 *
 *   public functions:
 *
 ***********************************************************************************/

void lcd_init_ssp(void) {
  ssp1__init(24);

  lcd.CLK = gpio__construct_with_function(0, 7, 2); // SCK1 => (0,7) func: 2
  lcd.DIN = gpio__construct_with_function(0, 9, 2); // MOSI1s => (0,8) func: 2
  lcd.CS = gpio__construct_as_output(2, 1);
  lcd.DC = gpio__construct_as_output(2, 4);
  lcd.RST = gpio__construct_as_output(2, 6);

  // LOW => ON; HIGH => OFF
  gpio__set(lcd.CS); // HIGH

  // LOW for Write command; HIGH for Write data
  gpio__set(lcd.DC); // HIGH

  // LOW => ON; HIGH => OFF
  gpio__set(lcd.RST); // HIGH

  lcd_hardware_reset(); // ENABLE RESET
  lcd_reg_init();

  delay__ms(200);
  lcd_write_command(0xAF); // Turn on the OLED display

  //   gpio__reset(lcd.CS);  // LOW

  //   lcd_write_command(0xAF); // Set Display ON
  //   lcd_write_command(0xA5);
}

void lcd_hardware_reset(void) {

  delay__ms(100);
  gpio__reset(lcd.RST);
  delay__ms(100);
  gpio__set(lcd.RST);
  delay__ms(100);
}

void lcd_write_command(uint8_t command) {
  gpio__reset(lcd.DC); // Write Command
  gpio__reset(lcd.CS); // Chip select ON.
  ssp1__transfer_byte(command);
  delay__ms(10);
  // END of function

  gpio__set(lcd.CS); // Chip select OFF.

  delay__ms(10);
}
void lcd_write_data(uint8_t data) {
  gpio__set(lcd.DC);   // Write Data
  gpio__reset(lcd.CS); // Chip select ON.
  ssp1__transfer_byte(data);
  delay__ms(100);
  // END of function

  gpio__set(lcd.CS); // Chip select OFF.

  delay__ms(10);
}

void lcd_display(char *input) {}