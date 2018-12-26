#ifndef __CRC_H__
#define __CRC_H__

#include "stm32l0xx.h"
#include "result.h"

result_t crc_8(uint8_t poly, uint8_t init, uint8_t* data, uint16_t length, uint8_t* result);

#endif
