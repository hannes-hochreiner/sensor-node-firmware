#ifndef __AES_H__
#define __AES_H__

#include "stm32l0xx.h"
#include "result.h"
#include "status.h"
#include "irq_handler.h"

// TODO: use interrupts to watch for key preparation completion

result_t aes_ecb_encrypt(uint32_t* data_in, uint32_t* data_out, uint8_t length);
result_t aes_ecb_decrypt(uint32_t* data_in, uint32_t* data_out, uint8_t length);

#endif
