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

result_t i2c_write(uint8_t address, uint8_t *data, uint8_t length) {
  uint8_t a = (address & 0x7F) << 1;

  status_dma_tx = STATUS_PENDING;

  DMA1_Channel4->CNDTR = length;
  DMA1_Channel4->CPAR = (uint32_t)((uint8_t*)&(I2C1->TXDR));
  DMA1_Channel4->CMAR = (uint32_t)data;
  DMA1_Channel4->CCR |= (DMA_CCR_EN);

  NVIC_EnableIRQ(DMA1_Channel4_5_6_7_IRQn);
  
  I2C1->CR1 |= I2C_CR1_PE;
  I2C1->CR2 = (I2C_CR2_AUTOEND) | (length << (I2C_CR2_NBYTES_Pos)) | (a << I2C_CR2_SADD_Pos);
  I2C1->CR2 |= (I2C_CR2_START);

  volatile uint32_t i2c_status = I2C1->ISR;

  if (((i2c_status & (I2C_ISR_BERR)) == (I2C_ISR_BERR)) || 
      ((i2c_status & (I2C_ISR_TIMEOUT)) == (I2C_ISR_TIMEOUT)) ||
      ((i2c_status & (I2C_ISR_NACKF)) == (I2C_ISR_NACKF)) ||
      ((i2c_status & (I2C_ISR_ARLO)) == (I2C_ISR_ARLO)) ||
      ((i2c_status & (I2C_ISR_OVR)) == (I2C_ISR_OVR))) {
    return RESULT_ERROR;
  }

  while (status_dma_tx == STATUS_PENDING) {
    __WFI();
  }

  while (((I2C1->ISR) & (I2C_ISR_STOPF)) != (I2C_ISR_STOPF)) {}

  NVIC_DisableIRQ(DMA1_Channel4_5_6_7_IRQn);
  DMA1_Channel4->CCR &= ~(DMA_CCR_EN);

  i2c_status = I2C1->ISR;

  if (((i2c_status & (I2C_ISR_BERR)) == (I2C_ISR_BERR)) || 
      ((i2c_status & (I2C_ISR_TIMEOUT)) == (I2C_ISR_TIMEOUT)) ||
      ((i2c_status & (I2C_ISR_NACKF)) == (I2C_ISR_NACKF)) ||
      ((i2c_status & (I2C_ISR_ARLO)) == (I2C_ISR_ARLO)) ||
      ((i2c_status & (I2C_ISR_OVR)) == (I2C_ISR_OVR))) {
    return RESULT_ERROR;
  }

  I2C1->CR1 &= ~(I2C_CR1_PE);

  if (status_dma_tx != STATUS_OK) {
    return RESULT_ERROR;
  }

  return RESULT_OK;
}

result_t i2c_read(uint8_t address, uint8_t *data, uint8_t length) {
  uint8_t a = (address & 0x7F) << 1;

  status_dma_rx = STATUS_PENDING;

  DMA1_Channel5->CNDTR = length;
  DMA1_Channel5->CPAR = (uint32_t)((uint8_t*)&(I2C1->RXDR));
  DMA1_Channel5->CMAR = (uint32_t)data;
  DMA1_Channel5->CCR |= (DMA_CCR_EN);

  NVIC_EnableIRQ(DMA1_Channel4_5_6_7_IRQn);

  I2C1->CR1 |= I2C_CR1_PE;
  I2C1->CR2 = (I2C_CR2_AUTOEND) | (length << (I2C_CR2_NBYTES_Pos)) | (a << I2C_CR2_SADD_Pos) | (I2C_CR2_RD_WRN);
  I2C1->CR2 |= (I2C_CR2_START);

  volatile uint32_t i2c_status = I2C1->ISR;

  if (((i2c_status & (I2C_ISR_BERR)) == (I2C_ISR_BERR)) || 
      ((i2c_status & (I2C_ISR_TIMEOUT)) == (I2C_ISR_TIMEOUT)) ||
      ((i2c_status & (I2C_ISR_NACKF)) == (I2C_ISR_NACKF)) ||
      ((i2c_status & (I2C_ISR_ARLO)) == (I2C_ISR_ARLO)) ||
      ((i2c_status & (I2C_ISR_OVR)) == (I2C_ISR_OVR))) {
    return RESULT_ERROR;
  }

  while (status_dma_rx == STATUS_PENDING) {
    __WFI();
  }

  while (((I2C1->ISR) & (I2C_ISR_STOPF)) != (I2C_ISR_STOPF)) {}

  NVIC_DisableIRQ(DMA1_Channel4_5_6_7_IRQn);
  DMA1_Channel5->CCR &= ~(DMA_CCR_EN);

  i2c_status = I2C1->ISR;

  if (((i2c_status & (I2C_ISR_BERR)) == (I2C_ISR_BERR)) || 
      ((i2c_status & (I2C_ISR_TIMEOUT)) == (I2C_ISR_TIMEOUT)) ||
      ((i2c_status & (I2C_ISR_NACKF)) == (I2C_ISR_NACKF)) ||
      ((i2c_status & (I2C_ISR_ARLO)) == (I2C_ISR_ARLO)) ||
      ((i2c_status & (I2C_ISR_OVR)) == (I2C_ISR_OVR))) {
    return RESULT_ERROR;
  }

  I2C1->CR1 &= ~(I2C_CR1_PE);

  if (status_dma_rx != STATUS_OK) {
    return RESULT_ERROR;
  }

  return RESULT_OK;
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
