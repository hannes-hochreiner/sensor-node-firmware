#ifndef __BME280_GLUE_H__
#define __BME280_GLUE_H__

#include "bme280.h"
#include "result.h"
#include "i2c.h"
#include "delay.h"
#include <string.h>

typedef struct {
  float humidity;
  float temperature;
  float pressure;
} bme280_g_data_t;

result_t bme280_g_init();
result_t bme280_g_measurement(bme280_g_data_t * data);

#endif
