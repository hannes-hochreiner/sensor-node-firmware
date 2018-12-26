#include "crc.h"

result_t crc_8(uint8_t poly, uint8_t init, uint8_t* data, uint16_t length, uint8_t* result) {
  RCC->AHBENR |= RCC_AHBENR_CRCEN;

  CRC->POL = (uint32_t)poly;
  CRC->INIT = (uint32_t)init;
  CRC->CR = (CRC->CR & ~(CRC_CR_POLYSIZE)) | (CRC_CR_POLYSIZE_1) | (CRC_CR_RESET);

  while ((CRC->CR & (CRC_CR_RESET)) == (CRC_CR_RESET)) {}

  for (uint16_t cntr = 0; cntr < length; cntr++) {
    *(uint8_t*)&(CRC->DR) = *(data + cntr);
  }

  *result = (CRC->DR);

  RCC->AHBENR &= ~(RCC_AHBENR_CRCEN);

  return RESULT_OK;
}
