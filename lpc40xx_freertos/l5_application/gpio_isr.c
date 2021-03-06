//
// CMPE 146
// Project: MP3
// Author: Yongming Li
//
#include "gpio_isr.h"

// Note: You may want another separate array for falling vs. rising edge
// callbacks
static function_pointer_t gpio0_callbacks[32];
static function_pointer_t gpio2_callbacks[32];

void gpio0__attach_interrupt(uint32_t pin, gpio_interrupt_e interrupt_type,
                             function_pointer_t callback) {
  if (interrupt_type == GPIO_INTR__FALLING_EDGE)
    LPC_GPIOINT->IO0IntEnF |= (1 << pin);
  else
    LPC_GPIOINT->IO0IntEnR |= (1 << pin); // GPIO_INTR__RISING_EDGE,

  // For callback
  gpio0_callbacks[pin] = callback;
}

void gpio0_2__interrupt_dispatcher(void) {
  for (int i = 0; i < 32; i++) {
    if ((LPC_GPIOINT->IO0IntStatR & (1 << i)) ||
        (LPC_GPIOINT->IO0IntStatF & (1 << i))) {
      gpio0_callbacks[i]();
      LPC_GPIOINT->IO0IntClr |= (1 << i); // clear interrupt
    }
  }
  for (int i = 0; i < 32; i++) {
    if ((LPC_GPIOINT->IO2IntStatR & (1 << i)) ||
        (LPC_GPIOINT->IO2IntStatF & (1 << i))) {
      gpio2_callbacks[i]();
      LPC_GPIOINT->IO2IntClr |= (1 << i); // clear interrupt
    }
  }
}

void gpio2__attach_interrupt(uint32_t pin, gpio_interrupt_e interrupt_type,
                             function_pointer_t callback) {
  if (interrupt_type == GPIO_INTR__FALLING_EDGE)
    LPC_GPIOINT->IO2IntEnF |= (1 << pin);
  else
    LPC_GPIOINT->IO2IntEnR |= (1 << pin); // GPIO_INTR__RISING_EDGE,

  // For callback
  gpio2_callbacks[pin] = callback;
}