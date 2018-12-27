#include "aes.h"

void aes_dma_c1_handler();
void aes_dma_c2_3_handler();

volatile status_t dma_tx;
volatile status_t dma_rx;

typedef enum {
  AES_ENCRYPT,
  AES_DECRYPT
} aes_dir_t;

result_t aes_ecb(aes_dir_t dir, aes_key_t* key, uint32_t* data_in, uint32_t* data_out, uint8_t length);

result_t aes_ecb_encrypt(aes_key_t* key, uint32_t* data_in, uint32_t* data_out, uint8_t length) {
  return aes_ecb(AES_ENCRYPT, key, data_in, data_out, length);
}

result_t aes_ecb_decrypt(aes_key_t* key, uint32_t* data_in, uint32_t* data_out, uint8_t length) {
  return aes_ecb(AES_DECRYPT, key, data_in, data_out, length);
}

result_t aes_ecb(aes_dir_t dir, aes_key_t* key, uint32_t* data_in, uint32_t* data_out, uint8_t length) {
  RCC->AHBENR |= (RCC_AHBENR_DMAEN) | (RCC_AHBENR_CRYPEN);
  // RCC->AHBRSTR = (RCC_AHBRSTR_DMARST) | (RCC_AHBRSTR_CRYPRST);
  // RCC->AHBRSTR = 0;
  // set key
  AES->KEYR0 = key->key0;
  AES->KEYR1 = key->key1;
  AES->KEYR2 = key->key2;
  AES->KEYR3 = key->key3;

  // decryption key preparation
  if (dir == AES_DECRYPT) {
    AES->CR = (0b01 << AES_CR_MODE_Pos) | AES_CR_EN;

    while ((AES->SR) == 0) {}

    if ((AES->SR) != AES_SR_CCF) {
      return RESULT_ERROR;
    }
  }

  dma_tx = STATUS_PENDING;
  dma_rx = STATUS_PENDING;

  // configure DMA: channel 1 tx, channel 2 rx
  DMA1_Channel1->CCR = (0b10 << (DMA_CCR_MSIZE_Pos)) | (0b10 << (DMA_CCR_PSIZE_Pos)) | (DMA_CCR_MINC) | (DMA_CCR_DIR) | (DMA_CCR_TEIE) | (DMA_CCR_TCIE);
  DMA1_Channel2->CCR = (0b10 << (DMA_CCR_MSIZE_Pos)) | (0b10 << (DMA_CCR_PSIZE_Pos)) | (DMA_CCR_MINC) | (DMA_CCR_TEIE) | (DMA_CCR_TCIE);
  DMA1_Channel1->CNDTR = length;
  DMA1_Channel2->CNDTR = length;
  DMA1_Channel1->CPAR = (uint32_t)&(AES->DINR);
  DMA1_Channel2->CPAR = (uint32_t)&(AES->DOUTR);
  DMA1_Channel1->CMAR = (uint32_t)data_in;
  DMA1_Channel2->CMAR = (uint32_t)data_out;
  DMA1_CSELR->CSELR = (0b1011 << (DMA_CSELR_C1S_Pos)) | (0b1011 << (DMA_CSELR_C2S_Pos));
  DMA1_Channel1->CCR |= (DMA_CCR_EN);
  DMA1_Channel2->CCR |= (DMA_CCR_EN);

  add_handler(DMA1_Channel1_IRQn, aes_dma_c1_handler);
  add_handler(DMA1_Channel2_3_IRQn, aes_dma_c2_3_handler);
  NVIC_EnableIRQ(DMA1_Channel1_IRQn);
  NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);

  // encryption
  if (dir == AES_DECRYPT) {
    AES->CR = (AES_CR_DMAOUTEN) | (AES_CR_DMAINEN) | AES_CR_EN | (0b10 << AES_CR_MODE_Pos);
  } else {
    AES->CR = (AES_CR_DMAOUTEN) | (AES_CR_DMAINEN) | AES_CR_EN;
  }

  while ((dma_tx == STATUS_PENDING) || (dma_rx == STATUS_PENDING)) {
    __WFI();
  }

  uint32_t status = (AES->SR);

  AES->CR = 0;

  DMA1_Channel1->CCR = 0;
  DMA1_Channel2->CCR = 0;
  NVIC_DisableIRQ(DMA1_Channel1_IRQn);
  NVIC_DisableIRQ(DMA1_Channel2_3_IRQn);
  remove_handler(DMA1_Channel1_IRQn, aes_dma_c1_handler);
  remove_handler(DMA1_Channel2_3_IRQn, aes_dma_c2_3_handler);

  if (status != (AES_SR_CCF)) {
    return RESULT_ERROR;
  }

  return RESULT_OK;
}

void aes_dma_c1_handler() {
  uint32_t status = (DMA1->ISR);

  DMA1->IFCR |= (DMA_IFCR_CGIF1);

  if ((status & DMA_ISR_TEIF1) == DMA_ISR_TEIF1) {
    dma_tx = STATUS_ERROR;
  } else if ((status & DMA_ISR_TCIF1) == DMA_ISR_TCIF1) {
    dma_tx = STATUS_OK;
  }
}

void aes_dma_c2_3_handler() {
  uint32_t status = (DMA1->ISR);

  DMA1->IFCR |= (DMA_IFCR_CGIF2);

  if ((status & DMA_ISR_TEIF2) == DMA_ISR_TEIF2) {
    dma_rx = STATUS_ERROR;
  } else if ((status & DMA_ISR_TCIF2) == DMA_ISR_TCIF2) {
    dma_rx = STATUS_OK;
  }
}
