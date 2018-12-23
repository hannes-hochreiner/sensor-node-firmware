#include "delay.h"

// static volatile status_t timer_status = STATUS_PENDING;
static volatile uint8_t counter;

void delay(uint16_t ms) {
  RCC->APB1ENR |= (RCC_APB1ENR_TIM2EN);
  // TIM2->CR1 |= (TIM_CR1_OPM);

  TIM2->DIER |= (TIM_DIER_UIE);

  // timer_status = STATUS_PENDING;
  counter = 0;
  TIM2->PSC = SystemCoreClock / 100000;
  TIM2->ARR = ms;
  TIM2->CNT = 0;
  NVIC_EnableIRQ(TIM2_IRQn);
  TIM2->CR1 |= (TIM_CR1_CEN);

  while (counter <= 100) {
    __WFI();
  }

  TIM2->CR1 &= ~(TIM_CR1_CEN);
  NVIC_DisableIRQ(TIM2_IRQn);
  RCC->APB1ENR &= ~(RCC_APB1ENR_TIM2EN);
}

void TIM2_IRQHandler() {
  uint16_t sr = (TIM2->SR);
  TIM2->SR = 0;

  if ((sr & TIM_SR_UIF) == TIM_SR_UIF) {
    // timer_status = STATUS_OK;
    counter++;
  }
}
