#include "main.h"

int main() {
  SystemCoreClockUpdate();

  // STM32L021
  uint32_t message_index = 0;

  rtc_init();
  spi_init();
  i2c_init();
  pin_reset_init();
  rfm9x_g_init();

  shtc3_t shtc3 = {
    i2c_read,
    i2c_write,
    crc_8,
    delay
  };

  while (1) {
    shtc3_data_t shtc3_data;
    message_0001_t msg;

    shtc3_get_data(&shtc3, &shtc3_data);

    msg.temperature = shtc3_data.temperature;
    msg.humidity = shtc3_data.humidity;
    msg.sensor_id = shtc3_data.id;
    msg.mcu_id_1 = *(uint32_t*)(UID_BASE);
    msg.mcu_id_2 = *(uint32_t*)(UID_BASE + 4);
    msg.mcu_id_3 = *(uint32_t*)(UID_BASE + 8);
    msg.message_index = message_index++;
    msg.type = 0x0001;
    msg._rng = (SysTick->VAL);

    uint8_t enc_data[32];
    aes_key_t key = {KEY0, KEY1, KEY2, KEY3};

    aes_ecb_encrypt(&key, (uint32_t*)&msg, (uint32_t*)enc_data, 8);

    rfm9x_g_send_message(enc_data, 32);

    stop_enable();
    rtc_wait_until_next_period();
    stop_disable();
  }
}
