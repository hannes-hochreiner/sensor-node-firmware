#include "rtc.h"

volatile static status_t rtc_status;

result_t rtc_init() {
  // start LSI
  RCC->CSR |= (RCC_CSR_LSION);
  // check whether LSI is ready
  while (((RCC->CSR) & (RCC_CSR_LSIRDY)) != (RCC_CSR_LSIRDY)) {}

  // set rtc clock source
  RCC->APB1ENR |= RCC_APB1ENR_PWREN;
  PWR->CR |= PWR_CR_DBP;
  RCC->CSR |= (RCC_CSR_RTCRST);
  RCC->CSR &= ~(RCC_CSR_RTCRST);
  RCC->CSR = ((RCC->CSR) & ~(RCC_CSR_RTCSEL)) | (0b10 << RCC_CSR_RTCSEL_Pos);
  RCC->CSR |= (RCC_CSR_RTCEN);

  // unlocking rtc registers
  RTC->WPR = 0xCA;
  RTC->WPR = 0x53;

  RTC->CR &= ~(RTC_CR_WUTE);

  while ((RTC->ISR & RTC_ISR_WUTWF) != RTC_ISR_WUTWF) {}

  // set rtc pre-scaler
  RTC->ISR |= RTC_ISR_INIT;

  while ((RTC->ISR & RTC_ISR_INITF) != RTC_ISR_INITF) {}

  RTC->PRER = (RTC->PRER & ~(RTC_PRER_PREDIV_S)) | ((286 & (RTC_PRER_PREDIV_S_Msk)) << RTC_PRER_PREDIV_S_Pos);

  RTC->ISR &= ~(RTC_ISR_INIT);

  // set wakeup-timer
  // RTC->WUTR = 60; // about 60 sec
  RTC->WUTR = 3; // about 3 sec
  RTC->CR = ((RTC->CR) & ~(RTC_CR_WUCKSEL)) | 0b100 << (RTC_CR_WUCKSEL_Pos) | (RTC_CR_WUTE) | (RTC_CR_WUTIE); /* (5) */

  // lock registers
  RTC->WPR = 0xFE;
  RTC->WPR = 0x64;

  // TODO: nvic
  NVIC_EnableIRQ(RTC_IRQn);

  // configure EXTI line 20
  EXTI->IMR |= (EXTI_IMR_IM20);
  EXTI->EMR |= (EXTI_EMR_EM20);
  EXTI->RTSR |= (EXTI_RTSR_RT20);

  return RESULT_OK;
}

result_t rtc_wait_until_next_period() {
  rtc_status = STATUS_PENDING;

  while (rtc_status == STATUS_PENDING) {
    __WFI();
  }

  if (rtc_status != STATUS_OK) {
    return RESULT_ERROR;
  }

  return RESULT_OK;
}

void RTC_IRQHandler() {
  EXTI->PR |= (EXTI_PR_PIF20);
  RTC->ISR &= ~(RTC_ISR_WUTF);

  rtc_status = STATUS_OK;
}