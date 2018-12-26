#include "rfm9x_glue.h"

volatile static rfm9x_t rfm9x;

result_t rfm9x_g_init() {
  rfm9x.set_reset_pin = pin_reset_set;
  rfm9x.reset_reset_pin = pin_reset_reset;
  rfm9x.delay = delay;
  rfm9x.spi_transfer = spi_transfer;

  RFM9X_Init(&rfm9x);
  uint8_t syncWord[] = {0x46, 0xA5, 0xE3};
  RFM9X_SetSyncWord(&rfm9x, syncWord, 3);
  uint8_t power = 0x08;
  RFM9X_SetPower(&rfm9x, &power);
  rfm9x_mode_t setMode = RFM9X_MODE_TRANSMIT;
  RFM9X_SetMode(&rfm9x, &setMode);

  return RESULT_OK;
}

result_t rfm9x_g_send_message(uint8_t* data, uint8_t length) {
  rfm9x_flags_t flags;

  RFM9X_WriteMessage(&rfm9x, data, length);
  RFM9X_GetFlags(&rfm9x, &flags);

  while ((flags & RFM9X_FLAG_PACKET_SENT) != RFM9X_FLAG_PACKET_SENT) {
    RFM9X_GetFlags(&rfm9x, &flags);
  }

  return RESULT_OK;
}
