#include "ssp2_lab.h"
#include "clock.h"
#include <stdbool.h>
#include <stddef.h>

#include "lpc40xx.h"
#include "lpc_peripherals.h"

// part 0

void ssp2__initia(uint32_t max_clock) {

  // Refer to LPC User manual and setup the register bits correctly

  // a) Power on Peripheral for SSP2;
  lpc_peripheral__turn_on_power_to(LPC_PERIPHERAL__SSP2); // ssp2=36;

  // b) Setup control registers CR0 and CR1

  LPC_SSP2->CR0 = 7; // 8-bit mode, look at table 483

  // Enable SSP. Table 484. set bit1 to 1
  LPC_SSP2->CR1 = (1 << 1); // Enable SSP. Table 484. set bit1 to 1
  // LPC_SSP2->CR1 = (0 << 2); //  SSP as Master. Table 484. bit2 to 0. reset is 0,
  //                           //  so do not need this.

  // c) Setup prescalar register to be <= max_clock_mhz
  // Table 487; This even value between 2 and 254, by which PCLK is divided to
  // yield the prescaler output clock. Finding the divider that smaller than
  // max_clock(mhz).
  // c) Setup prescalar register to be <= max_clock_mhz
  uint8_t divider = 2;
  const uint32_t cpu_clock_mhz = clock__get_core_clock_hz(); // in mhz
  // Keep scaling down divider until calculated is higher
  while (max_clock < (cpu_clock_mhz / divider) && divider <= 254)
  divider += 2;
  LPC_SSP2->CPSR = divider;
}

uint8_t ssp2__transfer_byte(uint8_t send) {
  // Configure the Data register(DR) to sendx and receive data by checking the
  // status register
  LPC_SSP2->DR = send;
  // BUSY:when bit4 set to 1, sending/receiving a frame.
  while (LPC_SSP2->SR & (1 << 4)) {
  }                    // wait until SSp2 is done
  return LPC_SSP2->DR; // Data Register
}
