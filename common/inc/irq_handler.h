#ifndef __IRQ_HANDLER_H__
#define __IRQ_HANDLER_H__

#include "stm32l021xx.h"
#include "result.h"

typedef void(*void_func)();

result_t add_handler(IRQn_Type num, void_func func);
result_t remove_handler(IRQn_Type num, void_func func);

#endif
