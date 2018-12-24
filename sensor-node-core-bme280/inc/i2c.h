#ifndef __I2C_H__
#define __I2C_H__

#include "stm32l0xx.h"
#include "status.h"
#include <string.h>

void i2c_init();
int8_t i2c_write(uint8_t dev_id, uint8_t reg_addr, uint8_t *data, uint16_t len);
int8_t i2c_read(uint8_t dev_id, uint8_t reg_addr, uint8_t *data, uint16_t len);

#endif
