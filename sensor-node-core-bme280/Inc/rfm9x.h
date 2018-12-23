#ifndef __rfm9x_H
#define __rfm9x_H
#include "stdio.h"
#include "stdint.h"

typedef enum {
  RFM9X_RESULT_OK,
  RFM9X_RESULT_ERROR
} rfm9x_result_t;
typedef rfm9x_result_t (*void_func)();
typedef rfm9x_result_t (*delay_func)(uint8_t millisec);
typedef rfm9x_result_t (*spi_transfer_func)(uint8_t* const data);
typedef rfm9x_result_t (*read_func)(const uint8_t* const data, uint8_t length);
typedef struct {
  void_func set_reset_pin;
  void_func reset_reset_pin;
  void_func set_spi_nss_pin;
  void_func reset_spi_nss_pin;
  delay_func delay;
  spi_transfer_func spi_transfer;
} rfm9x_t;
typedef enum {
  RFM9X_MODE_SLEEP = 0x00,
  RFM9X_MODE_STANDBY = 0x01,
  RFM9X_MODE_TRANSMIT = 0x03,
  RFM9X_MODE_RECEIVE = 0x05
} rfm9x_mode_t;
typedef enum {
  RFM9X_FLAG_MODE_READY = 0x8000,
  RFM9X_FLAG_RX_READY = 0x4000,
  RFM9X_FLAG_TX_READY = 0x2000,
  RFM9X_FLAG_PLL_LOCK = 0x1000,
  RFM9X_FLAG_RSSI = 0x0800,
  RFM9X_FLAG_TIMEOUT = 0x0400,
  RFM9X_FLAG_PREAMBLE_DETECT = 0x0200,
  RFM9X_FLAG_SYNC_ADDRESS_MATCH = 0x0100,
  RFM9X_FLAG_FIFO_FULL = 0x0080,
  RFM9X_FLAG_FIFO_EMPTY = 0x0040,
  RFM9X_FLAG_FIFO_LEVEL = 0x0020,
  RFM9X_FLAG_FIFO_OVERRUN = 0x0010,
  RFM9X_FLAG_PACKET_SENT = 0x0008,
  RFM9X_FLAG_PAYLOAD_READY = 0x0004,
  RFM9X_FLAG_CRC_OK = 0x0002,
  RFM9X_FLAG_LOW_BAT = 0x0001
} rfm9x_flags_t;
typedef enum {
  RFM9X_CRC_AUTOCLEAR_ON = 0xF7,
  RFM9X_CRC_AUTOCLEAR_OFF = 0x08
} rfm9x_crc_autoclear_mode_t;
typedef enum {
  RFM9X_FREQ_DEV_5KHZ = 0x0052,
  RFM9X_FREQ_DEV_50KHZ = 0x0334
} rfm9x_freq_dev_t;

rfm9x_result_t RFM9X_Init(const rfm9x_t* const rfm9x);
rfm9x_result_t RFM9X_Reset(const rfm9x_t* const rfm9x);
rfm9x_result_t RFM9X_GetVersion(const rfm9x_t* const rfm9x, uint8_t* const version);
rfm9x_result_t RFM9X_GetFrequency(const rfm9x_t* const rfm9x, uint32_t* const frequencyHz);
rfm9x_result_t RFM9X_GetSyncWord(const rfm9x_t* const rfm9x, uint8_t syncWord[8], uint8_t* const length);
rfm9x_result_t RFM9X_SetSyncWord(const rfm9x_t* const rfm9x, const uint8_t* const syncWord, uint8_t length);
rfm9x_result_t RFM9X_GetMode(const rfm9x_t* const rfm9x, rfm9x_mode_t* const mode);
rfm9x_result_t RFM9X_SetMode(const rfm9x_t* const rfm9x, const rfm9x_mode_t* const mode);
rfm9x_result_t RFM9X_GetFlags(const rfm9x_t* const rfm9x, uint16_t* const flags);
rfm9x_result_t RFM9X_SetBitrate(const rfm9x_t* const rfm9x, const uint32_t* const bitrate);
rfm9x_result_t RFM9X_GetBitrate(const rfm9x_t* const rfm9x, uint32_t* const bitrate);
rfm9x_result_t RFM9X_WriteMessage(const rfm9x_t* const rfm9x, uint8_t* const data, uint8_t length);
rfm9x_result_t RFM9X_ReadMessage(const rfm9x_t* const rfm9x, read_func callback);
rfm9x_result_t RFM9X_SetCrcAutoClearMode(const rfm9x_t* const rfm9x, const rfm9x_crc_autoclear_mode_t* const mode);
rfm9x_result_t RFM9X_SetFreqDev(const rfm9x_t* const rfm9x, const rfm9x_freq_dev_t* const freqDev);
rfm9x_result_t RFM9X_SetPower(const rfm9x_t* const rfm9x, const uint8_t* const outputPower);
rfm9x_result_t RFM9X_GetPower(const rfm9x_t* const rfm9x, uint8_t* const outputPower);
rfm9x_result_t RFM9X_GetRSSIValue(const rfm9x_t* const rfm9x, uint8_t* const rssiValue);
#endif
