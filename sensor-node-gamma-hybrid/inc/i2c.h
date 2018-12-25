#ifndef __I2C_H__
#define __I2C_H__

#include "stm32l0xx.h"
#include "status.h"
#include "result.h"
#include <string.h>

void i2c_init();
result_t i2c_write(uint8_t address, uint8_t *data, uint8_t length);
result_t i2c_read(uint8_t address, uint8_t *data, uint8_t length);

#endif
