#include "i2c.h"

static status_t status_dma = STATUS_PENDING;

void i2c_disable();
void i2c_dma_c4_5_handler();

#define I2C_ERROR_FLAGS ((I2C_ISR_BERR) | (I2C_ISR_TIMEOUT) | (I2C_ISR_NACKF) | (I2C_ISR_ARLO) | (I2C_ISR_OVR))

typedef enum {
  I2C_TRANSFER_READ,
  I2C_TRANSFER_WRITE
} i2c_transfer_t;

result_t i2c_transfer(i2c_transfer_t type, uint8_t address, uint8_t *data, uint8_t length);

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

  // timing: 0x708 for 2.097 MHz clock in normal mode (100 kHz)
  I2C1->CR1 = (I2C_CR1_TXDMAEN) | (I2C_CR1_RXDMAEN);
  I2C1->TIMINGR = (uint32_t)0x00000708;
}

result_t i2c_write(uint8_t address, uint8_t *data, uint8_t length) {
  return i2c_transfer(I2C_TRANSFER_WRITE, address, data, length);
}

result_t i2c_read(uint8_t address, uint8_t *data, uint8_t length) {
  return i2c_transfer(I2C_TRANSFER_READ, address, data, length);
}

result_t i2c_transfer(i2c_transfer_t type, uint8_t address, uint8_t *data, uint8_t length) {
  uint8_t a = (address & 0x7F) << 1;
  DMA_Channel_TypeDef* dma_channel;
  uint8_t* data_register;
  uint32_t direction_flag;

  status_dma = STATUS_PENDING;

  // tx: dma channel 4
  // rx: dma channel 5
  DMA1_Channel4->CCR |= (DMA_CCR_MINC) | (DMA_CCR_TEIE) | (DMA_CCR_TCIE) | (DMA_CCR_DIR);
  DMA1_Channel5->CCR |= (DMA_CCR_MINC) | (DMA_CCR_TEIE) | (DMA_CCR_TCIE);
  DMA1_CSELR->CSELR |= (0b0110 << (DMA_CSELR_C4S_Pos)) | (0b0110 << (DMA_CSELR_C5S_Pos));

  if (type == I2C_TRANSFER_READ) {
    dma_channel = DMA1_Channel5;
    data_register = (uint8_t*)&(I2C1->RXDR);
    direction_flag = (I2C_CR2_RD_WRN);
  } else if (type == I2C_TRANSFER_WRITE) {
    dma_channel = DMA1_Channel4;
    data_register = (uint8_t*)&(I2C1->TXDR);
    direction_flag = 0;
  } else {
    i2c_disable();
    return RESULT_ERROR;
  }

  dma_channel->CNDTR = length;
  dma_channel->CPAR = (uint32_t)data_register;
  dma_channel->CMAR = (uint32_t)data;
  dma_channel->CCR |= (DMA_CCR_EN);

  add_handler(DMA1_Channel4_5_IRQn, i2c_dma_c4_5_handler);
  NVIC_EnableIRQ(DMA1_Channel4_5_IRQn);
  
  I2C1->CR1 |= I2C_CR1_PE;
  I2C1->CR2 = (I2C_CR2_AUTOEND) | (length << (I2C_CR2_NBYTES_Pos)) | (a << I2C_CR2_SADD_Pos) | direction_flag;
  I2C1->CR2 |= (I2C_CR2_START);

  if (((I2C1->ISR) & I2C_ERROR_FLAGS) != 0) {
    i2c_disable();
    return RESULT_ERROR;
  }

  while (status_dma == STATUS_PENDING) {
    __WFI();
  }

  while (((I2C1->ISR) & (I2C_ISR_BUSY)) == (I2C_ISR_BUSY)) {}

  uint32_t i2c_status = (I2C1->ISR);

  i2c_disable();

  if (((i2c_status & I2C_ERROR_FLAGS) != 0) ||
      (status_dma != STATUS_OK) ||
      ((i2c_status & (I2C_ISR_STOPF)) != (I2C_ISR_STOPF))) {
    return RESULT_ERROR;
  }

  return RESULT_OK;
}

void i2c_disable() {
  DMA1_Channel4->CCR = 0;
  DMA1_Channel5->CCR = 0;
  NVIC_DisableIRQ(DMA1_Channel4_5_IRQn);
  remove_handler(DMA1_Channel4_5_IRQn, i2c_dma_c4_5_handler);
  I2C1->CR1 &= ~(I2C_CR1_PE);
}

void i2c_dma_c4_5_handler() {
  uint32_t status = (DMA1->ISR);
  
  DMA1->IFCR |= DMA_IFCR_CGIF4;
  DMA1->IFCR |= DMA_IFCR_CGIF5;

  if ((status & (DMA_ISR_TEIF4)) == (DMA_ISR_TEIF4)) {
    status_dma = STATUS_ERROR;
  } else if ((status & (DMA_ISR_TCIF4)) == (DMA_ISR_TCIF4)) {
    status_dma = STATUS_OK;
  }
  
  if ((status & (DMA_ISR_TEIF5)) == (DMA_ISR_TEIF5)) {
    status_dma = STATUS_ERROR;
  } else if ((status & (DMA_ISR_TCIF5)) == (DMA_ISR_TCIF5)) {
    status_dma = STATUS_OK;
  }
}
