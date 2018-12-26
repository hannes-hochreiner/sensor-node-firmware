#ifndef __SPI_H__
#define __SPI_H__

#include "stm32l0xx.h"
#include "status.h"

void spi_init();
void spi_transfer(uint8_t* const data, uint16_t length);

#endif
