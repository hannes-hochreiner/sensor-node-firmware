#include "i2c.h"

volatile static status_t status_dma_tx;
volatile static status_t status_dma_rx;

// PB6 SCL
// PB7 SDA

void i2c_init() {
  RCC->AHBENR |= (RCC_AHBENR_DMAEN);
  RCC->APB1ENR |= (RCC_APB1ENR_I2C1EN);
  RCC->IOPENR |= (RCC_IOPENR_IOPBEN);

  // pins: open-drain, very high speed
  GPIOB->MODER = (GPIOB->MODER & ~(GPIO_MODER_MODE6)) | (GPIO_MODER_MODE6_1);
  GPIOB->MODER = (GPIOB->MODER & ~(GPIO_MODER_MODE7)) | (GPIO_MODER_MODE7_1);
  GPIOB->OTYPER |= (GPIO_OTYPER_OT_6);
  GPIOB->OTYPER |= (GPIO_OTYPER_OT_7);
  GPIOB->OSPEEDR = (GPIOB->OSPEEDR & ~(GPIO_OSPEEDER_OSPEED6)) | (GPIO_OSPEEDER_OSPEED6_0) | (GPIO_OSPEEDER_OSPEED6_1);
  GPIOB->OSPEEDR = (GPIOB->OSPEEDR & ~(GPIO_OSPEEDER_OSPEED7)) | (GPIO_OSPEEDER_OSPEED7_0) | (GPIO_OSPEEDER_OSPEED7_1);
  GPIOB->AFR[0] = (GPIOB->AFR[0] & ~(GPIO_AFRL_AFSEL6)) | (0b0001 << GPIO_AFRL_AFSEL6_Pos);
  GPIOB->AFR[0] = (GPIOB->AFR[0] & ~(GPIO_AFRL_AFSEL7)) | (0b0001 << GPIO_AFRL_AFSEL7_Pos);

  // tx: dma channel 4
  // rx: dma channel 5
  DMA1_Channel4->CCR |= (DMA_CCR_MINC) | (DMA_CCR_TEIE) | (DMA_CCR_TCIE) | (DMA_CCR_DIR);
  DMA1_Channel5->CCR |= (DMA_CCR_MINC) | (DMA_CCR_TEIE) | (DMA_CCR_TCIE);
  DMA1_CSELR->CSELR |= (0b0110 << (DMA_CSELR_C4S_Pos)) | (0b0110 << (DMA_CSELR_C5S_Pos));

  // timing: 0x708 for 2.097 MHz clock in normal mode (100 kHz)
  I2C1->CR1 = (I2C_CR1_TXDMAEN) | (I2C_CR1_RXDMAEN);
  I2C1->TIMINGR = (uint32_t)0x00000708;
}

int8_t i2c_write(uint8_t dev_id, uint8_t reg_addr, uint8_t *data, uint16_t len) {
  uint8_t l = (uint8_t)len;
  uint8_t a = (dev_id & 0x7F) << 1;
  uint8_t d[l + 1];
  d[0] = reg_addr;
  memcpy((d + 1), data, l);

  status_dma_tx = STATUS_PENDING;

  DMA1_Channel4->CNDTR = l;
  DMA1_Channel4->CPAR = (uint32_t)(&(I2C1->TXDR));
  DMA1_Channel4->CMAR = (uint32_t)d;
  DMA1_Channel4->CCR |= (DMA_CCR_EN);

  NVIC_EnableIRQ(DMA1_Channel4_5_6_7_IRQn);

  I2C1->CR1 |= I2C_CR1_PE;
  I2C1->CR2 = (I2C_CR2_AUTOEND) | (l << (I2C_CR2_NBYTES_Pos)) | (I2C_CR2_START) | (a << I2C_CR2_SADD_Pos);

  while (status_dma_tx == STATUS_PENDING) {
    __WFI();
  }

  I2C1->CR1 &= ~(I2C_CR1_PE);
  NVIC_DisableIRQ(DMA1_Channel4_5_6_7_IRQn);
  DMA1_Channel4->CCR &= ~(DMA_CCR_EN);

  if (status_dma_tx == STATUS_OK) {
    return 0;
  }

  return 1;
}

int8_t i2c_read(uint8_t dev_id, uint8_t reg_addr, uint8_t *data, uint16_t len) {
  uint8_t l = (uint8_t)len;
  volatile uint8_t a = (dev_id & 0x7F) << 1;

  status_dma_tx = STATUS_PENDING;
  status_dma_rx = STATUS_PENDING;

  DMA1_Channel4->CNDTR = 1;
  DMA1_Channel4->CPAR = (uint32_t)(&(I2C1->TXDR));
  DMA1_Channel4->CMAR = (uint32_t)&reg_addr;
  DMA1_Channel4->CCR |= (DMA_CCR_EN);

  DMA1_Channel5->CNDTR = l;
  DMA1_Channel5->CPAR = (uint32_t)(&(I2C1->RXDR));
  DMA1_Channel5->CMAR = (uint32_t)data;
  DMA1_Channel5->CCR |= (DMA_CCR_EN);

  NVIC_EnableIRQ(DMA1_Channel4_5_6_7_IRQn);

  I2C1->CR1 |= I2C_CR1_PE;
  I2C1->CR2 = (1 << (I2C_CR2_NBYTES_Pos)) | (I2C_CR2_START) | (a << I2C_CR2_SADD_Pos);

  while (status_dma_tx == STATUS_PENDING) {
    __WFI();
  }

  I2C1->CR2 = (I2C_CR2_AUTOEND) | (l << (I2C_CR2_NBYTES_Pos)) | (I2C_CR2_START) | (a << I2C_CR2_SADD_Pos) | (I2C_CR2_RD_WRN);

  while (status_dma_rx == STATUS_PENDING) {
    __WFI();
  }

  I2C1->CR1 &= ~(I2C_CR1_PE);
  NVIC_DisableIRQ(DMA1_Channel4_5_6_7_IRQn);
  DMA1_Channel4->CCR &= ~(DMA_CCR_EN);
  DMA1_Channel5->CCR &= ~(DMA_CCR_EN);

  if ((status_dma_tx == STATUS_OK) && (status_dma_rx == STATUS_OK)) {
    return 0;
  }

  return 1;
}

void DMA1_Channel4_5_6_7_IRQHandler() {
  uint32_t status = (DMA1->ISR);
  
  DMA1->IFCR |= DMA_IFCR_CGIF4;
  DMA1->IFCR |= DMA_IFCR_CGIF5;

  if ((status & (DMA_ISR_TEIF4)) == (DMA_ISR_TEIF4)) {
    status_dma_tx = STATUS_ERROR;
  }
  if ((status & (DMA_ISR_TCIF4)) == (DMA_ISR_TCIF4)) {
    status_dma_tx = STATUS_OK;
  }
  if ((status & (DMA_ISR_TEIF5)) == (DMA_ISR_TEIF5)) {
    status_dma_rx = STATUS_ERROR;
  }
  if ((status & (DMA_ISR_TCIF5)) == (DMA_ISR_TCIF5)) {
    status_dma_rx = STATUS_OK;
  }
}
