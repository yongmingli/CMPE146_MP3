#pragma once

#include <stdint.h>
#include <stdlib.h>

void ssp2__initia(uint32_t max_clock);
uint8_t ssp2__transfer_byte(uint8_t data_out);
void Find_max_clock();
