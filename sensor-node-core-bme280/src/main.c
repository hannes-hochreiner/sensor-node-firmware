#include "main.h"

int main() {
  SystemCoreClockUpdate();

  // STM32L021

  spi_init();
  pin_reset_init();
  i2c_init();
  // bme280_g_init();
  rfm9x_g_init();

  while (1) {
    // volatile bme280_g_data_t meas_data;
    // volatile result_t res_meas = bme280_g_measurement(&meas_data);
    volatile uint8_t address = 0b0011001;
    uint8_t comId[] = {0x0F};
    uint8_t dataId[] = {0x00};

    // result_t i2c_res = i2c_write(address, comId, 1);
    // i2c_res = i2c_read(address, dataId, 1);

    uint8_t text[] = "Hello World! Hello World!";
    rfm9x_g_send_message(text, 25);

    delay(500);
  }
}
