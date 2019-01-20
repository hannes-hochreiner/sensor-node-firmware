#include "rfm9x_glue.h"

volatile static rfm9x_t rfm98;

result_t rfm9x_g_init() {
  rfm98.set_reset_pin = pin_reset_set;
  rfm98.reset_reset_pin = pin_reset_reset;
  rfm98.delay = delay;
  rfm98.spi_transfer = spi_transfer;

  RFM9X_Init(&rfm98);
  RFM9X_Reset(&rfm98);

  rfm9x_mode_t mode = RFM9X_MODE_LOW_FREQ_MODE | RFM9X_MODE_SLEEP;
  
  RFM9X_SetMode(&rfm98, &mode);
  mode = RFM9X_MODE_LORA | RFM9X_MODE_LOW_FREQ_MODE | RFM9X_MODE_SLEEP;
  RFM9X_SetMode(&rfm98, &mode);
  mode = RFM9X_MODE_LORA | RFM9X_MODE_LOW_FREQ_MODE | RFM9X_MODE_STANDBY;
  RFM9X_SetMode(&rfm98, &mode);

  rfm9x_conf1_t conf1 = RFM9X_CONF1_BW_125_KHZ | RFM9X_CONF1_CR_4_5 | RFM9X_CONF1_HEADER_EXPL;
  RFM9X_SetConf1(&rfm98, &conf1);
  rfm9x_conf2_t conf2 = RFM9X_CONF2_SF_128;
  RFM9X_SetConf2(&rfm98, &conf2);
  rfm9x_conf3_t conf3 = RFM9X_CONF3_AGC_AUTO_ON;
  RFM9X_SetConf3(&rfm98, &conf3);

  uint8_t power = 0x00;
  RFM9X_SetPower(&rfm98, &power);

  return RESULT_OK;
}

result_t rfm9x_g_send_message(uint8_t* data, uint8_t length) {
  rfm9x_mode_t mode = RFM9X_MODE_LORA | RFM9X_MODE_LOW_FREQ_MODE | RFM9X_MODE_STANDBY;
  RFM9X_SetMode(&rfm98, &mode);

  rfm9x_flags_t flags;
  rfm9x_flags_t flags_all = RFM9X_FLAG_ALL;

  RFM9X_WriteMessage(&rfm98, data, length);
  
  mode = RFM9X_MODE_LORA | RFM9X_MODE_LOW_FREQ_MODE | RFM9X_MODE_TRANSMIT;
  RFM9X_SetMode(&rfm98, &mode);
  RFM9X_GetMode(&rfm98, &mode);
  RFM9X_GetFlags(&rfm98, &flags);
  
  while (((flags & RFM9X_FLAG_TX_DONE) != RFM9X_FLAG_TX_DONE) ||
         ((mode & 0x07) != RFM9X_MODE_STANDBY)) {
    RFM9X_GetMode(&rfm98, &mode);
    RFM9X_GetFlags(&rfm98, &flags);
    delay(25);
  }

  RFM9X_ResetFlags(&rfm98, &flags_all);
  mode = RFM9X_MODE_LORA | RFM9X_MODE_LOW_FREQ_MODE | RFM9X_MODE_SLEEP;
  RFM9X_SetMode(&rfm98, &mode);

  return RESULT_OK;
}
