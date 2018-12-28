#include "system.h"

void stop_enable() {
  SCB->SCR |= (1 << SCB_SCR_SLEEPDEEP_Pos);
  PWR->CR |= (PWR_CR_ULP) | (PWR_CR_LPDS_Pos);
}

void stop_disable() {
  PWR->CR &= ~((PWR_CR_ULP) | (PWR_CR_LPDS_Pos));
  SCB->SCR &= ~(1 << SCB_SCR_SLEEPDEEP_Pos);
}
