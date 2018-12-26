#include "bme280_glue.h"

volatile static struct bme280_dev bme280;

void bme280_g_delay(uint32_t ms);
int8_t bme280_g_i2c_read(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len);
int8_t bme280_g_i2c_write(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len);

result_t bme280_g_init() {
  bme280.dev_id = BME280_I2C_ADDR_PRIM;
  bme280.intf = BME280_I2C_INTF;
  bme280.read = bme280_g_i2c_read;
  bme280.write = bme280_g_i2c_write;
  bme280.delay_ms = bme280_g_delay;
  bme280.settings.osr_h = BME280_OVERSAMPLING_1X;
  bme280.settings.osr_p = BME280_OVERSAMPLING_1X;
  bme280.settings.osr_t = BME280_OVERSAMPLING_1X;
  bme280.settings.filter = BME280_FILTER_COEFF_OFF;

  if (bme280_init(&bme280) != BME280_OK) {
    return RESULT_ERROR;
  }

  if (bme280_set_sensor_settings(BME280_OSR_PRESS_SEL | BME280_OSR_TEMP_SEL | BME280_OSR_HUM_SEL | BME280_FILTER_SEL, &bme280) != BME280_OK) {
    return RESULT_ERROR;
  }

  return RESULT_OK;
}

result_t bme280_g_measurement(bme280_g_data_t * data) {
  uint8_t bme280Mode = BME280_FORCED_MODE;

  if (bme280_set_sensor_mode(bme280Mode, &bme280) != BME280_OK) {
    return RESULT_ERROR;
  }

  while (bme280Mode != BME280_SLEEP_MODE) {
    delay(1);

    if (bme280_get_sensor_mode(&bme280Mode, &bme280) != BME280_OK) {
      return RESULT_ERROR;
    }
  }

  struct bme280_data bme280_data;

  if (bme280_get_sensor_data(BME280_ALL, &bme280_data, &bme280) != BME280_OK) {
    return RESULT_ERROR;
  }

  data->pressure = (float)bme280_data.pressure / 100;
  data->temperature = (float)bme280_data.temperature / 100;
  data->humidity = (float)bme280_data.humidity / 1024;

  return RESULT_OK;
}

void bme280_g_delay(uint32_t ms) {
  uint16_t h = (uint16_t)(ms >> 16);
  uint16_t l = (uint16_t)(ms & 0x0000FFFF);

  for (uint16_t cntr = 0; cntr < h; cntr++) {
    delay(0xFFFF);
  }

  delay(l);
}

int8_t bme280_g_i2c_read(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len) {
  result_t res = i2c_write(dev_id, &reg_addr, 1);

  if (res != RESULT_OK) {
    return 1;
  }

  res = i2c_read(dev_id, reg_data, len);

  if (res != RESULT_OK) {
    return 1;
  }

  return 0;
}

int8_t bme280_g_i2c_write(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len) {
  uint8_t d[len + 1];

  d[0] = reg_addr;
  memcpy((d + 1), reg_data, len);

  result_t res = i2c_write(dev_id, d, len + 1);

  if (res != RESULT_OK) {
    return 1;
  }

  return 0;
}
