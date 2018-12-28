#ifndef __RTC_H__
#define __RTC_H__

#include "stm32l0xx.h"
#include "result.h"
#include "status.h"

result_t rtc_init();
result_t rtc_wait_until_next_period();

#endif
