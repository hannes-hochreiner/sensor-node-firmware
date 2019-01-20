#ifndef __AES_H__
#define __AES_H__

#include "stm32l0xx.h"
#include "result.h"
#include "status.h"
#include "irq_handler.h"

// TODO: use interrupts to watch for key preparation completion

typedef struct {
  uint32_t key0;
  uint32_t key1;
  uint32_t key2;
  uint32_t key3;
} aes_key_t;

result_t aes_ecb_encrypt(aes_key_t* key, uint32_t* data_in, uint32_t* data_out, uint8_t length);
result_t aes_ecb_decrypt(aes_key_t* key, uint32_t* data_in, uint32_t* data_out, uint8_t length);

#endif
