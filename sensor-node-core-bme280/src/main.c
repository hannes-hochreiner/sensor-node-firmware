#include "main.h"

int main() {
  SystemCoreClockUpdate();

  // STM32L021

  spi_init();
  pin_reset_init();
  i2c_init();

  struct bme280_dev bme280;
  volatile int8_t rslt = BME280_OK;

  bme280.dev_id = BME280_I2C_ADDR_PRIM;
  bme280.intf = BME280_I2C_INTF;
  bme280.read = i2c_read;
  bme280.write = i2c_write;
  bme280.delay_ms = delay;
  bme280.settings.osr_h = BME280_OVERSAMPLING_1X;
  bme280.settings.osr_p = BME280_OVERSAMPLING_1X;
  bme280.settings.osr_t = BME280_OVERSAMPLING_1X;
  bme280.settings.filter = BME280_FILTER_COEFF_OFF;

  rslt = bme280_init(&bme280);
  rslt = bme280_set_sensor_settings(BME280_OSR_PRESS_SEL | BME280_OSR_TEMP_SEL | BME280_OSR_HUM_SEL | BME280_FILTER_SEL, &bme280);

  rslt = bme280_crc_selftest(&bme280);

  rfm9x_t rfm98 = {
    pin_reset_set,
    pin_reset_reset,
    delay,
    spi_transfer
  };

  RFM9X_Init(&rfm98);
  uint8_t syncWord[] = {0x46, 0xA5, 0xE3};
  RFM9X_SetSyncWord(&rfm98, syncWord, 3);
  uint8_t power = 0x08;
  RFM9X_SetPower(&rfm98, &power);

  rfm9x_flags_t flags;
  RFM9X_GetFlags(&rfm98, &flags);

  rfm9x_mode_t setMode = RFM9X_MODE_TRANSMIT;
  RFM9X_SetMode(&rfm98, &setMode);

  while (1) {
    volatile struct bme280_data comp_data;

    bme280_soft_reset(&bme280);
    uint8_t bme280Mode = BME280_FORCED_MODE;

    rslt = bme280_set_sensor_mode(bme280Mode, &bme280);

    while (bme280Mode != BME280_SLEEP_MODE) {
      delay(1);
      rslt = bme280_get_sensor_mode(&bme280Mode, &bme280);
    }

    rslt = bme280_get_sensor_data(BME280_ALL, &comp_data, &bme280);

    volatile float press = (float)comp_data.pressure / 100;
    volatile float temp = (float)comp_data.temperature / 100;
    volatile float hum = (float)comp_data.humidity / 1024;

    uint8_t text[] = "Hello World! Hello World!";
    RFM9X_WriteMessage(&rfm98, text, 25);

    RFM9X_GetFlags(&rfm98, &flags);

    while ((flags & RFM9X_FLAG_PACKET_SENT) != RFM9X_FLAG_PACKET_SENT) {
      RFM9X_GetFlags(&rfm98, &flags);
    }

    delay(1000);
  }
}
