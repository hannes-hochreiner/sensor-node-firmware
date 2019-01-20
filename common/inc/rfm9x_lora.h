#ifndef __RFM9X_LORA_H__
#define __RFM9X_LORA_H__
#include "stdio.h"
#include "stdint.h"
#include <string.h>

typedef void (*void_func)();
typedef void (*delay_func)(uint16_t millisec);
typedef void (*spi_transfer_func)(uint8_t* const data, uint16_t length);
typedef void (*read_func)(const uint8_t* const data, uint16_t length);
typedef struct {
  void_func set_reset_pin;
  void_func reset_reset_pin;
  delay_func delay;
  spi_transfer_func spi_transfer;
} rfm9x_t;
typedef enum {
  RFM9X_MODE_SLEEP = 0x00,
  RFM9X_MODE_STANDBY = 0x01,
  RFM9X_MODE_TRANSMIT = 0x03,
  RFM9X_MODE_RECEIVE_CONT = 0x05,
  RFM9X_MODE_LOW_FREQ_MODE = 0x08,
  RFM9X_MODE_LORA = 0x80
} rfm9x_mode_t;
typedef enum {
  RFM9X_FLAG_ALL = 0xFF,
  RFM9X_FLAG_RX_TIMEOUT = 0x80,
  RFM9X_FLAG_RX_DONE = 0x40,
  RFM9X_FLAG_PAYLOAD_CRC_ERROR = 0x20,
  RFM9X_FLAG_VALID_HEADER = 0x10,
  RFM9X_FLAG_TX_DONE = 0x08,
  RFM9X_FLAG_CAD_DONE = 0x04,
  RFM9X_FLAG_FHSS_CHANGE_CHANNEL = 0x02,
  RFM9X_FLAG_CAD_DETECT = 0x01,
} rfm9x_flags_t;
typedef enum {
  RFM9X_CONF1_BW_7_8_KHZ = 0x00,
  RFM9X_CONF1_BW_10_4_KHZ = 0x10,
  RFM9X_CONF1_BW_15_6_KHZ = 0x20,
  RFM9X_CONF1_BW_20_8_KHZ = 0x30,
  RFM9X_CONF1_BW_31_25_KHZ = 0x40,
  RFM9X_CONF1_BW_41_7_KHZ = 0x50,
  RFM9X_CONF1_BW_62_5_KHZ = 0x60,
  RFM9X_CONF1_BW_125_KHZ = 0x70,
  RFM9X_CONF1_BW_250_KHZ = 0x80,
  RFM9X_CONF1_BW_500_KHZ = 0x90,
  RFM9X_CONF1_CR_4_5 = 0x02,
  RFM9X_CONF1_CR_4_6 = 0x04,
  RFM9X_CONF1_CR_4_7 = 0x06,
  RFM9X_CONF1_CR_4_8 = 0x08,
  RFM9X_CONF1_HEADER_IMPL = 0x01,
  RFM9X_CONF1_HEADER_EXPL = 0x00
} rfm9x_conf1_t;
typedef enum {
  RFM9X_CONF2_SF_64 = 0x60,
  RFM9X_CONF2_SF_128 = 0x70,
  RFM9X_CONF2_SF_256 = 0x80,
  RFM9X_CONF2_SF_512 = 0x90,
  RFM9X_CONF2_SF_1024 = 0xA0,
  RFM9X_CONF2_SF_2048 = 0xB0,
  RFM9X_CONF2_SF_4096 = 0xC0,
} rfm9x_conf2_t;
typedef enum {
  RFM9X_CONF3_AGC_AUTO_ON = 0x04,
  RFM9X_CONF3_MOBILE_NODE = 0x08
} rfm9x_conf3_t;

void RFM9X_Init(const rfm9x_t* const rfm9x);
void RFM9X_Reset(const rfm9x_t* const rfm9x);
void RFM9X_SetMode(const rfm9x_t* const rfm9x, const rfm9x_mode_t* const mode);
void RFM9X_GetMode(const rfm9x_t* const rfm9x, rfm9x_mode_t* const mode);
void RFM9X_SetConf1(const rfm9x_t* const rfm9x, const rfm9x_conf1_t* const conf1);
void RFM9X_SetConf2(const rfm9x_t* const rfm9x, const rfm9x_conf2_t* const conf2);
void RFM9X_SetConf3(const rfm9x_t* const rfm9x, const rfm9x_conf3_t* const conf3);
void RFM9X_GetFlags(const rfm9x_t* const rfm9x, rfm9x_flags_t* const flags);
void RFM9X_ResetFlags(const rfm9x_t* const rfm9x, const rfm9x_flags_t* const flags);
void RFM9X_WriteMessage(const rfm9x_t* const rfm9x, uint8_t* const data, uint8_t length);
void RFM9X_ReadMessage(const rfm9x_t* const rfm9x, read_func callback);
void RFM9X_SetPower(const rfm9x_t* const rfm9x, const uint8_t* const outputPower);
void RFM9X_GetRssiPacket(const rfm9x_t* const rfm9x, int* const rssi);
#endif
