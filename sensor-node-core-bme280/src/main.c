#include "main.h"

int main() {
  SystemCoreClockUpdate();

  uint32_t message_index = 0;

  rtc_init();
  spi_init();
  i2c_init();
  pin_reset_init();
  bme280_g_init();
  rfm9x_g_init();

  while (1) {
    bme280_g_data_t meas_data;
    result_t res_meas = bme280_g_measurement(&meas_data);
    message_0002_t msg;

    msg.temperature = meas_data.temperature;
    msg.humidity = meas_data.humidity;
    msg.pressure = meas_data.pressure;
    msg.sensor_id = SENSOR_ID;
    msg.mcu_id_1 = *(uint32_t*)(UID_BASE);
    msg.mcu_id_2 = *(uint32_t*)(UID_BASE + 4);
    msg.mcu_id_3 = *(uint32_t*)(UID_BASE + 8);
    msg.message_index = message_index++;
    msg.type = 0x0002;

    uint8_t enc_data[32];
    aes_key_t key = {KEY0, KEY1, KEY2, KEY3};

    aes_ecb_encrypt(&key, (uint32_t*)&msg, (uint32_t*)enc_data, 8);

    rfm9x_g_send_message(enc_data, 32);

    stop_enable();
    rtc_wait_until_next_period();
    stop_disable();
  }
}
