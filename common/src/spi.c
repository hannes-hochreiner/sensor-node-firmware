#include "spi.h"

static volatile status_t status_dma_tx = STATUS_PENDING;
static volatile status_t status_dma_rx = STATUS_PENDING;

void spi_dma_c2_3_handler();

void spi_init() {
  RCC->AHBENR |= (RCC_AHBENR_DMAEN);
  RCC->APB2ENR |= (RCC_APB2ENR_SPI1EN);
  RCC->IOPENR |= (RCC_IOPENR_IOPAEN);
  RCC->IOPENR |= (RCC_IOPENR_IOPBEN);

  // PA7 (MOSI), PB0 (MISO), and PA5 (SCK) are initialized to "alternate function push/pull", without pull ups/downs, at very high speed.
  // PA4 (NCS) is initialized to push/pull
  GPIOA->MODER = (GPIOA->MODER & ~(GPIO_MODER_MODE4)) | (GPIO_MODER_MODE4_0);
  GPIOA->MODER = (GPIOA->MODER & ~(GPIO_MODER_MODE5)) | (GPIO_MODER_MODE5_1);
  GPIOB->MODER = (GPIOB->MODER & ~(GPIO_MODER_MODE0)) | (GPIO_MODER_MODE0_1);
  GPIOA->MODER = (GPIOA->MODER & ~(GPIO_MODER_MODE7)) | (GPIO_MODER_MODE7_1);
  GPIOA->OSPEEDR = (GPIOA->OSPEEDR & ~(GPIO_OSPEEDER_OSPEED4)) | (GPIO_OSPEEDER_OSPEED4_0) | (GPIO_OSPEEDER_OSPEED4_1);
  GPIOA->OSPEEDR = (GPIOA->OSPEEDR & ~(GPIO_OSPEEDER_OSPEED5)) | (GPIO_OSPEEDER_OSPEED5_0) | (GPIO_OSPEEDER_OSPEED5_1);
  GPIOB->OSPEEDR = (GPIOB->OSPEEDR & ~(GPIO_OSPEEDER_OSPEED0)) | (GPIO_OSPEEDER_OSPEED0_0) | (GPIO_OSPEEDER_OSPEED0_1);
  GPIOA->OSPEEDR = (GPIOA->OSPEEDR & ~(GPIO_OSPEEDER_OSPEED7)) | (GPIO_OSPEEDER_OSPEED7_0) | (GPIO_OSPEEDER_OSPEED7_1);

  GPIOB->AFR[0] = (GPIOB->AFR[0] & ~(GPIO_AFRL_AFSEL0)) | (0b0001 << GPIO_AFRL_AFSEL0_Pos);

  GPIOA->BSRR = (GPIO_BSRR_BS_4);
 
  SPI1->CR1 = ((SPI_CR1_MSTR) | (SPI_CR1_BR) | (SPI_CR1_SSM) | (SPI_CR1_SSI));
  SPI1->CR2 = ((SPI_CR2_TXDMAEN) | (SPI_CR2_RXDMAEN));
}

void spi_transfer(uint8_t* const data, uint16_t length) {
  // RX channel 2
  // TX channel 3
  // set CMAR (MA), CPAR (PA), CNDTR (NDT), CCR (EN)
  DMA1_Channel2->CCR = (DMA_CCR_MINC) | (DMA_CCR_TEIE) | (DMA_CCR_TCIE);
  DMA1_Channel3->CCR = (DMA_CCR_MINC) | (DMA_CCR_TEIE) | (DMA_CCR_TCIE) | (DMA_CCR_DIR);
  DMA1_CSELR->CSELR = (1 << (DMA_CSELR_C2S_Pos)) | (1 << (DMA_CSELR_C3S_Pos));

  DMA1_Channel2->CNDTR = length;
  DMA1_Channel3->CNDTR = length;
  DMA1_Channel2->CPAR = (uint32_t)(&(SPI1->DR));
  DMA1_Channel3->CPAR = (uint32_t)(&(SPI1->DR));
  DMA1_Channel2->CMAR = (uint32_t)data;
  DMA1_Channel3->CMAR = (uint32_t)data;
  DMA1_Channel2->CCR |= (DMA_CCR_EN);
  DMA1_Channel3->CCR |= (DMA_CCR_EN);

  status_dma_tx = STATUS_PENDING;
  status_dma_rx = STATUS_PENDING;

  add_handler(DMA1_Channel2_3_IRQn, spi_dma_c2_3_handler);
  NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);
  
  SPI1->CR1 |= (SPI_CR1_SPE);
  GPIOA->BSRR = GPIO_BSRR_BR_4;

  while ((status_dma_rx == STATUS_PENDING) || (status_dma_tx == STATUS_PENDING)) {
    __WFI();
  }

  GPIOA->BSRR = GPIO_BSRR_BS_4;
  SPI1->CR1 &= ~(SPI_CR1_SPE);

  DMA1_Channel2->CCR = 0;
  DMA1_Channel3->CCR = 0;
  NVIC_DisableIRQ(DMA1_Channel2_3_IRQn);
  remove_handler(DMA1_Channel2_3_IRQn, spi_dma_c2_3_handler);
}

void spi_dma_c2_3_handler() {
  uint32_t status = (DMA1->ISR);
  
  DMA1->IFCR |= DMA_IFCR_CGIF2;
  DMA1->IFCR |= DMA_IFCR_CGIF3;

  if ((status & (DMA_ISR_TEIF2)) == (DMA_ISR_TEIF2)) {
    status_dma_rx = STATUS_ERROR;
  } else if ((status & (DMA_ISR_TCIF2)) == (DMA_ISR_TCIF2)) {
    status_dma_rx = STATUS_OK;
  }
  
  if ((status & (DMA_ISR_TEIF3)) == (DMA_ISR_TEIF3)) {
    status_dma_tx = STATUS_ERROR;
  } else if ((status & (DMA_ISR_TCIF3)) == (DMA_ISR_TCIF3)) {
    status_dma_tx = STATUS_OK;
  }
}
