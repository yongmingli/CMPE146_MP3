//
// CMPE 146
// Project: MP3
// Author: Yongming Li
//
#include "lcd.h"

/***********************************************************************************
 *
 *   private functions:
 *
 ***********************************************************************************/

void lcd_pins(void);
void write_command(int command);
void execute(void);

void lcd_pins(void) {
  E = gpio__construct_as_output(1, 30);
  RS = gpio__construct_as_output(1, 31);
  DB0 = gpio__construct_as_output(1, 20);
  DB1 = gpio__construct_as_output(1, 23);
  DB2 = gpio__construct_as_output(1, 28);
  DB3 = gpio__construct_as_output(1, 29);
  DB4 = gpio__construct_as_output(2, 1);
  DB5 = gpio__construct_as_output(2, 4);
  DB6 = gpio__construct_as_output(2, 6);
  DB7 = gpio__construct_as_output(2, 8);

  //   gpio__set_function(E, 0);
  //   gpio__set_function(RS, 0);

  //   gpio__set_function(DB0, 0);
  //   gpio__set_function(DB1, 0);
  //   gpio__set_function(DB2, 0);
  //   gpio__set_function(DB3, 0);
  //   gpio__set_function(DB4, 0);
  //   gpio__set_function(DB5, 0);
  //   gpio__set_function(DB6, 0);
  //   gpio__set_function(DB7, 0);
}

void write_command(int command) {
  gpio__reset(RS); // COMMAND

  int n = command;
  int c, d;
  int t = 7;
  int ascii_bin[8] = {0};

  for (c = 7; c >= 0; c--) {
    d = n >> c;
    if (d & 1)
      ascii_bin[t] = 1;
    else
      ascii_bin[t] = 0;
    printf("at bit %i is: %i\n", t, ascii_bin[t]);
    t--;
  }

  if (ascii_bin[0] == 1)
    gpio__set(DB0);
  else
    gpio__reset(DB0);
  if (ascii_bin[1] == 1)
    gpio__set(DB1);
  else
    gpio__reset(DB1);
  if (ascii_bin[2] == 1)
    gpio__set(DB2);
  else
    gpio__reset(DB2);
  if (ascii_bin[3] == 1)
    gpio__set(DB3);
  else
    gpio__reset(DB3);
  if (ascii_bin[4] == 1)
    gpio__set(DB4);
  else
    gpio__reset(DB4);
  if (ascii_bin[5] == 1)
    gpio__set(DB5);
  else
    gpio__reset(DB5);
  if (ascii_bin[6] == 1)
    gpio__set(DB6);
  else
    gpio__reset(DB6);
  if (ascii_bin[7] == 1)
    gpio__set(DB7);
  else
    gpio__reset(DB7);

  gpio__set(E);
  delay__ms(10);
  gpio__reset(E);
  //   gpio__set(E);
}

void execute(void) {
  gpio__set(E);
  // delay__ms(10);
  gpio__reset(E);
  gpio__set(E);
}

/***********************************************************************************
 *
 *   END of private functions
 *
 ***********************************************************************************/

void lcd_init(void) {

  lcd_pins();
  delay__ms(10);
  display_on();
  delay__ms(10);
  display_clear();
  delay__ms(10);
  set_2_lines();
  delay__ms(10);
  first_line();

  //   display_on();
  //   //   write_command(56); // 0x38
  //   //   write_command(12); // 0x0c
  //   //   write_command(6);  // 0x06

  //   // delay__ms(10);
  //   display_clear();
  //   // delay__ms(10);

  display("WELCOME");

  //   lcd.clearDisplay();
  // delay_ms(10);
  // lcd.firstLine();
  // delay_ms(10);
  // lcd.display(">:"+song_lists[current_song]);
  // delay_ms(50);
  // reader.openFile(song_lists[current_song]);
  // lcd.nextLine();
  // delay_ms(10);
  // lcd.display("Next:"+song_lists[next_song]);
  // delay_ms(50);
}

void gpio__set(gpio_s gpio);   ///< Sets the pin value as 'high' -> 3.3v
void gpio__reset(gpio_s gpio); ///< Sets the pin value as 'low' -> ground

void display_on(void) {

  gpio__reset(RS); // COMMAND

  gpio__reset(DB0);
  gpio__reset(DB1);
  gpio__set(DB2);
  gpio__set(DB3);
  gpio__reset(DB4);
  gpio__reset(DB5);
  gpio__reset(DB6);
  gpio__reset(DB7);

  execute();
}
void display_clear(void) { // 0x01

  gpio__reset(RS); // COMMAND

  gpio__set(DB0);
  gpio__reset(DB1);
  gpio__reset(DB2);
  gpio__reset(DB3);
  gpio__reset(DB4);
  gpio__reset(DB5);
  gpio__reset(DB6);
  gpio__reset(DB7);

  execute();
}
void display(char *input) {
  //   gpio__set(RS); // DATA

  int size = strlen(input);
  int ascii_dec = 0;
  for (int i = 0; i < size; i++) {
    ascii_dec = input[i];
    printf("The decimal is %i\n", ascii_dec);

    int n = ascii_dec;
    int c, d;
    int t = 7;
    int ascii_bin[8] = {0};

    for (c = 7; c >= 0; c--) {
      d = n >> c;
      if (d & 1)
        ascii_bin[t] = 1;
      else
        ascii_bin[t] = 0;
      printf("at bit %i is: %i\n", t, ascii_bin[t]);
      t--;
    }

    if (ascii_bin[0] == 1)
      gpio__set(DB0);
    else
      gpio__reset(DB0);
    if (ascii_bin[1] == 1)
      gpio__set(DB1);
    else
      gpio__reset(DB1);
    if (ascii_bin[2] == 1)
      gpio__set(DB2);
    else
      gpio__reset(DB2);
    if (ascii_bin[3] == 1)
      gpio__set(DB3);
    else
      gpio__reset(DB3);
    if (ascii_bin[4] == 1)
      gpio__set(DB4);
    else
      gpio__reset(DB4);
    if (ascii_bin[5] == 1)
      gpio__set(DB5);
    else
      gpio__reset(DB5);
    if (ascii_bin[6] == 1)
      gpio__set(DB6);
    else
      gpio__reset(DB6);
    if (ascii_bin[7] == 1)
      gpio__set(DB7);
    else
      gpio__reset(DB7);

    gpio__set(RS); // DATA

    execute();
  }
}

void set_2_lines(void) {
  gpio__reset(RS); // COMMAND

  gpio__set(DB2);
  gpio__set(DB3);
  gpio__set(DB4);
  gpio__set(DB5);
  gpio__reset(DB6);
  gpio__reset(DB7);

  execute();
}

void next_line(void) {
  gpio__reset(RS); // COMMAND

  gpio__reset(DB0);
  gpio__reset(DB1);
  gpio__reset(DB2);
  gpio__set(DB3);
  gpio__reset(DB4);
  gpio__set(DB5);
  gpio__reset(DB6);
  gpio__set(DB7);

  execute();
}

void first_line(void) {

  gpio__reset(RS); // COMMAND

  gpio__reset(DB0);
  gpio__reset(DB1);
  gpio__reset(DB2);
  gpio__reset(DB3);
  gpio__reset(DB4);
  gpio__reset(DB5);
  gpio__reset(DB6);
  gpio__set(DB7);

  execute();
}