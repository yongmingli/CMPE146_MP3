//
// CMPE 146
// Project: MP3
// Author: Yongming Li
//
#pragma once

#include <stdint.h>
#include <stdlib.h>

void ssp1__init(uint32_t max_clock);

uint8_t ssp1__transfer_byte(uint8_t sned);
