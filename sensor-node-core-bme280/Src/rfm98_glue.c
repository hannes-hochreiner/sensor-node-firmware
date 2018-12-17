#include "rfm98_glue.h"

void RFM98Glue_Init(rfm9x_t* const rfm98) {
  rfm98->set_reset_pin = RFM98Glue_Set_Reset_Pin;
  rfm98->reset_reset_pin = RFM98Glue_Reset_Reset_Pin;
  rfm98->set_spi_nss_pin = RFM98Glue_Set_NSS_Pin;
  rfm98->reset_spi_nss_pin = RFM98Glue_Reset_NSS_Pin;
  rfm98->spi_transfer = RFM98Glue_SPI_Transfer;
  rfm98->delay = RFM98Glue_Delay;

  LL_SPI_Enable(SPI1);
}

void RFM98Glue_Set_Reset_Pin() {
  LL_GPIO_SetOutputPin(RFM_RESET_GPIO_Port, RFM_RESET_Pin);
}

void RFM98Glue_Reset_Reset_Pin() {
  LL_GPIO_ResetOutputPin(RFM_RESET_GPIO_Port, RFM_RESET_Pin);
}

void RFM98Glue_Set_NSS_Pin() {
  LL_GPIO_SetOutputPin(SPI1_NCS_GPIO_Port, SPI1_NCS_Pin);
}

void RFM98Glue_Reset_NSS_Pin() {
  LL_GPIO_ResetOutputPin(SPI1_NCS_GPIO_Port, SPI1_NCS_Pin);
}

void RFM98Glue_SPI_Transfer(uint8_t* const data) {
  // LL_SPI_EnableIT_TXE(SPI1);
  // while (!LL_SPI_IsActiveFlag_TXE(SPI1)) {
  //   __WFI();
  // }
  while (!LL_SPI_IsActiveFlag_TXE(SPI1)) {}
  LL_SPI_TransmitData8(SPI1, *data);
  while (!LL_SPI_IsActiveFlag_TXE(SPI1)) {}
  while (!LL_SPI_IsActiveFlag_RXNE(SPI1)) {}
  *data = LL_SPI_ReceiveData8(SPI1);
  while (LL_SPI_IsActiveFlag_RXNE(SPI1)) {}
}

void RFM98Glue_Delay(uint8_t millisec) {
  LL_mDelay(millisec);
}
