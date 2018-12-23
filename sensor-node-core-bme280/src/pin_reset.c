#include "pin_reset.h"

void pin_reset_init() {
  // set IOPCEN of RCC_IOPENR (GPIO port A enable)
  RCC->IOPENR |= RCC_IOPENR_IOPAEN;
  // set MODE3 of GPIOA_MODER to 01 (output push-pull on PC3)
  GPIOA->MODER = (GPIOA->MODER & ~GPIO_MODER_MODE3) | GPIO_MODER_MODE3_0;
  // set OT3 of GPIOA_OTYPER to 0 (set PC3 to output push-pull)
  GPIOA->OTYPER &= ~GPIO_OTYPER_OT_3;
  // set OSPEED3 of GPIOC_OSPEEDR to 11 (set very high speed for PC3)
  GPIOA->OSPEEDR = (GPIOA->OSPEEDR & ~GPIO_OSPEEDER_OSPEED3) | GPIO_OSPEEDER_OSPEED3_1 | GPIO_OSPEEDER_OSPEED3_1;
}

void pin_reset_set() {
  GPIOA->BSRR = GPIO_BSRR_BS_3;
}

void pin_reset_reset() {
  GPIOA->BSRR = GPIO_BSRR_BS_3;
}
