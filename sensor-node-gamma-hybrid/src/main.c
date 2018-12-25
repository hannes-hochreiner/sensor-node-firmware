#include "main.h"

int main() {
  SystemCoreClockUpdate();

  // STM32L021

  spi_init();
  pin_reset_init();
  i2c_init();

  shtc3_t shtc3 = {
    SHTC3_ADDRESS,
    i2c_read,
    i2c_write,
    crc_8
  };

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
    volatile shtc3_data_t shtc3_data;

    shtc3_get_data(&shtc3, &shtc3_data);

    uint8_t text[] = "Hello World! Hello World!";
    RFM9X_WriteMessage(&rfm98, text, 25);

    RFM9X_GetFlags(&rfm98, &flags);

    while ((flags & RFM9X_FLAG_PACKET_SENT) != RFM9X_FLAG_PACKET_SENT) {
      RFM9X_GetFlags(&rfm98, &flags);
    }

    delay(1000);
  }
}
