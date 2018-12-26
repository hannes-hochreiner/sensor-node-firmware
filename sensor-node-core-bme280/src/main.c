#include "main.h"

int main() {
  SystemCoreClockUpdate();

  // STM32L021

  spi_init();
  pin_reset_init();
  i2c_init();
  bme280_g_init();
  rfm9x_g_init();

  while (1) {
    volatile bme280_g_data_t meas_data;
    volatile result_t res_meas = bme280_g_measurement(&meas_data);

    uint8_t text[] = "Hello World! Hello World!";
    rfm9x_g_send_message(text, 25);

    delay(1000);
  }
}
