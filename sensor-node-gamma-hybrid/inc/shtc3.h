#ifndef __SHTC3_C__
#define __SHTC3_C__

#include "stdint.h"
#include "result.h"

#define SHTC3_ADDRESS 0x70

typedef result_t (*shtc3_read_func)(uint8_t address, uint8_t *data, uint8_t length);
typedef result_t (*shtc3_write_func)(uint8_t address, uint8_t *data, uint8_t length);
typedef result_t (*shtc3_crc_func)(uint8_t poly, uint8_t init, uint8_t* data, uint16_t length, uint8_t* result);
typedef struct {
  uint8_t address;
  shtc3_read_func i2c_read;
  shtc3_write_func i2c_write;
  shtc3_crc_func crc_8;
} shtc3_t;

typedef struct {
  uint16_t id;
  float humidity;
  float temperature;
} shtc3_data_t;

result_t shtc3_get_data(const shtc3_t*const shtc3, shtc3_data_t*const data);

#endif
