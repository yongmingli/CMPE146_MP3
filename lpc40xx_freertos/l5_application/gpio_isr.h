//
// CMPE 146
// Project: MP3
// Author: Yongming Li
//
#include "FreeRTOS.h"
#include "delay.h"
#include "gpio.h"
#include "lpc40xx.h"
#include "lpc_peripherals.h"
#include "semphr.h"
#include "task.h"
#include "uart.h"
#include "uart_printf.h"

typedef enum {
  GPIO_INTR__FALLING_EDGE,
  GPIO_INTR__RISING_EDGE,
} gpio_interrupt_e;

// Function pointer type (demonstrated later in the code sample)
typedef void (*function_pointer_t)(void);

// Allow the user to attach their callbacks
void gpio0__attach_interrupt(uint32_t pin, gpio_interrupt_e interrupt_type,
                             function_pointer_t callback);

// Our main() should configure interrupts to invoke this dispatcher where we
// will invoke user attached callbacks
void gpio0_2__interrupt_dispatcher(void);

// Allow the user to attach their callbacks
void gpio2__attach_interrupt(uint32_t pin, gpio_interrupt_e interrupt_type,
                             function_pointer_t callback);