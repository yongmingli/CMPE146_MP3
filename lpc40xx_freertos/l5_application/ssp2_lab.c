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
  uint8_t divider = 2;
  const uint32_t cpu_clock_mhz = clock__get_core_clock_hz(); // in mhz
  // Keep scaling down divider until calculated is higher
  while (max_clock < (cpu_clock_mhz / divider) && divider <= 254)
    divider += 2;
  LPC_SSP2->CPSR = divider;
}

// void Find_max_clock(uint32_t max_clock) {
//   // In master mode, CPSDVSRmin = 2 or larger (even numbers only).
//   uint8_t divider = 2;
//   const uint32_t cpu_clock = clock__get_core_clock_hz();
//   // Keep scaling down divider until calculated is higher
//   // Finding the divider that smaller than max_clock(mhz).
//   while (max_clock < (cpu_clock / divider) && divider <= 254) {
//     divider = divider + 2;
//   }
// }

uint8_t ssp2__transfer_byte(uint8_t send) {
  // Configure the Data register(DR) to sendx and receive data by checking the
  // status register
  LPC_SSP2->DR = send;
  // BUSY:when bit4 set to 1, sending/receiving a frame.
  while (LPC_SSP2->SR & (1 << 4)) {
  }                    // wait until SSp2 is done
  return LPC_SSP2->DR; // Data Register
}
