#ifndef __RFM9X_GLUE_H__
#define __RFM9X_GLUE_H__

#include "rfm9x.h"
#include "spi.h"
#include "delay.h"
#include "pin_reset.h"
#include "result.h"

result_t rfm9x_g_init();
result_t rfm9x_g_send_message(uint8_t* data, uint8_t length);

#endif
