#include "main.h"

int main() {
  SystemCoreClockUpdate();

  // STM32L021
  uint32_t message_index = 0;

  rtc_init();
  spi_init();
  pin_reset_init();
  i2c_init();

  shtc3_t shtc3 = {
    i2c_read,
    i2c_write,
    crc_8,
    delay
  };

  rfm9x_t rfm98 = {
    pin_reset_set,
    pin_reset_reset,
    delay,
    spi_transfer
  };

  RFM9X_Init(&rfm98);
  RFM9X_Reset(&rfm98);
  uint8_t syncWord[] = {0x46, 0xA5, 0xE3};
  RFM9X_SetSyncWord(&rfm98, syncWord, 3);
  uint8_t power = 0x08;
  RFM9X_SetPower(&rfm98, &power);
  // rfm9x_bit_rate_t bitrate = RFM9X_BIT_RATE_12K5;
  // RFM9X_SetBitrate(&rfm98, &bitrate);

  rfm9x_flags_t flags;
  RFM9X_GetFlags(&rfm98, &flags);

  rfm9x_mode_t setMode = RFM9X_MODE_SLEEP;
  RFM9X_SetMode(&rfm98, &setMode);

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

    RFM9X_WriteMessage(&rfm98, enc_data, 32);
    setMode = RFM9X_MODE_TRANSMIT;
    RFM9X_SetMode(&rfm98, &setMode);
    RFM9X_GetFlags(&rfm98, &flags);

    while ((flags & RFM9X_FLAG_PACKET_SENT) != RFM9X_FLAG_PACKET_SENT) {
      RFM9X_GetFlags(&rfm98, &flags);
    }

    setMode = RFM9X_MODE_SLEEP;
    RFM9X_SetMode(&rfm98, &setMode);

    stop_enable();
    rtc_wait_until_next_period();
    stop_disable();
  }
}
