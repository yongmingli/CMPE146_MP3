#include "ssp_lcd.h"
#include "delay.h"
#include "freertos_interrupt_handlers.h"
#include "gpio.h"
#include "lpc40xx.h"
#include "lcd_ssp.h"

void lcd_init(void){
    ssp1__init(24); 

    lcd.CLK = gpio__construct_with_function(0, 7, 2); // SCK1 => (0,7) func: 2
    lcd.DIN = gpio__construct_with_function(0, 9, 2);  // MOSI1s => (0,8) func: 2
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

}

void lcd_hardware_reset(void) {

  delay__ms(100);
  gpio__reset(lcd.RST);
  delay__ms(100);
  gpio__set(lcd.RST);
  delay__ms(100);
}

void lcd_write_command(){

}
void lcd_write_data(){

}
void lcd_display(char *input){
    
}