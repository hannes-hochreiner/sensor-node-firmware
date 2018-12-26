#ifndef __SHTC3_C__
#define __SHTC3_C__

#include "stdint.h"
#include "result.h"

typedef enum {
  SHTC3_RESULT_OK,
  SHTC3_RESULT_ERROR_I2C,
  SHTC3_RESULT_ERROR_I2C_WRITE_WAKEUP,
  SHTC3_RESULT_ERROR_I2C_WRITE_ID,
  SHTC3_RESULT_ERROR_I2C_WRITE_MEAS,
  SHTC3_RESULT_ERROR_I2C_WRITE_SLEEP,
  SHTC3_RESULT_ERROR_I2C_READ_ID,
  SHTC3_RESULT_ERROR_I2C_READ_MEAS,
  SHTC3_RESULT_ERROR_CRC
} shtc3_result_t;

typedef void (*shtc3_delay)(uint16_t ms);
typedef result_t (*shtc3_read_func)(uint8_t address, uint8_t *data, uint8_t length);
typedef result_t (*shtc3_write_func)(uint8_t address, uint8_t *data, uint8_t length);
typedef result_t (*shtc3_crc_func)(uint8_t poly, uint8_t init, uint8_t* data, uint16_t length, uint8_t* result);
typedef struct {
  shtc3_read_func i2c_read;
  shtc3_write_func i2c_write;
  shtc3_crc_func crc_8;
  shtc3_delay delay;
} shtc3_t;

typedef struct {
  uint16_t id;
  float humidity;
  float temperature;
} shtc3_data_t;

shtc3_result_t shtc3_get_data(const shtc3_t*const shtc3, shtc3_data_t*const data);

#endif
