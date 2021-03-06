//
// CMPE 146
// Project: MP3
// Author: Yongming Li
//
#include "lcd_ssp.h"
#include "clock.h"
#include <stdbool.h>
#include <stddef.h>

#include "lpc40xx.h"
#include "lpc_peripherals.h"

// part 0

void ssp1__init(uint32_t max_clock) {

  // Refer to LPC User manual and setup the register bits correctly

  // a) Power on Peripheral for SSP2;
  lpc_peripheral__turn_on_power_to(LPC_PERIPHERAL__SSP1);

  // b) Setup control registers CR0 and CR1

  LPC_SSP1->CR0 = 7; // 8-bit mode, look at table 483
  uint8_t divider = 2;
  const uint32_t cpu_clock_mhz = clock__get_core_clock_hz(); // in mhz
  // Keep scaling down divider until calculated is higher
  while (max_clock < (cpu_clock_mhz / divider) && divider <= 254)
    divider += 2;
  LPC_SSP1->CPSR = divider;
}

uint8_t ssp1__transfer_byte(uint8_t send) {
  // Configure the Data register(DR) to sendx and receive data by checking the
  // status register
  LPC_SSP1->DR = send;
  // BUSY:when bit4 set to 1, sending/receiving a frame.
  while (LPC_SSP1->SR & (1 << 4)) {
  }                    // wait until SSp2 is done
  return LPC_SSP1->DR; // Data Register
}
