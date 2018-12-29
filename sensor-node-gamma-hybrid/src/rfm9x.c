#include "rfm9x.h"

const uint8_t RFM9X_READ = 0x00;
const uint8_t RFM9X_WRITE = 0x80;

const uint8_t RFM9X_REG_FIFO = 0x00;
const uint8_t RFM9X_REG_MODE = 0x01;
const uint8_t RFM9X_REG_BITRATE_MSB = 0x02;
const uint8_t RFM9X_REG_BITRATE_LSB = 0x03;
const uint8_t RFM9X_REG_FDEV_MSB = 0x04;
const uint8_t RFM9X_REG_FDEV_LSB = 0x05;
const uint8_t RFM9X_REG_FREQUENCY = 0x06;
const uint8_t RFM9X_REG_PA_CONFIG = 0x09;
const uint8_t RFM9X_REG_RSSI_VALUE = 0x11;
const uint8_t RFM9X_REG_SYNC_CONFIG = 0x27;
const uint8_t RFM9X_REG_PACKET_CONFIG_1 = 0x30;
const uint8_t RFM9X_REG_IRQ_FLAGS = 0x3e;
const uint8_t RFM9X_REG_VERSION = 0x42;
const uint8_t RFM9X_REG_BITRATE_FRAC = 0x5d; // bits 0-3; 4-7 are reserved

void RFM9X_WriteMessage(const rfm9x_t* const rfm9x, uint8_t* const data, uint8_t length) {
  uint8_t d[length + 2];

  d[0] = RFM9X_WRITE | RFM9X_REG_FIFO;
  d[1] = length;

  memcpy((d + 2), data, length);

  rfm9x->spi_transfer(d, length + 2);
}

void RFM9X_ReadMessage(const rfm9x_t* const rfm9x, read_func callback) {
  uint8_t d[2];

  d[0] = RFM9X_READ | RFM9X_REG_FIFO;
  d[1] = 0;

  rfm9x->spi_transfer(d, 2);

  uint8_t data[d[1]];

  rfm9x->spi_transfer(data, d[1]);

  callback(data, d[1]);
}

void RFM9X_SetBitrate(const rfm9x_t* const rfm9x, const rfm9x_bit_rate_t* const bitrate) {
  uint8_t d[3];

  d[0] = RFM9X_WRITE | RFM9X_REG_BITRATE_MSB;
  memcpy((d + 1), bitrate, 2);

  rfm9x->spi_transfer(d, 3);
}

/*
 * Bitrate (b/s) = FXOSC (32 MHz) / (Bitrate + BitrateFrac / 16)
 * e.g. 0x1A0B = ~4.8 kb/s
*/
// rfm9x_result_t RFM9X_GetBitrate(const rfm9x_t* const rfm9x, uint32_t* const bitrate) {
//   uint8_t com = RFM9X_READ | RFM9X_REG_BITRATE_MSB;
//   uint8_t tmp;

//   check(rfm9x->reset_spi_nss_pin())
//   check(rfm9x->spi_transfer(&com))
//   check(rfm9x->spi_transfer(&tmp))
//   *bitrate = tmp << 8;
//   check(rfm9x->spi_transfer(&tmp))
//   check(rfm9x->set_spi_nss_pin())
//   *bitrate += tmp;
//   *bitrate = 32000000 / *bitrate;

//   return RFM9X_RESULT_OK;
// }

// rfm9x_result_t RFM9X_GetVersion(const rfm9x_t* const rfm9x, uint8_t* const version) {
//   uint8_t com = RFM9X_READ | RFM9X_REG_VERSION;

//   check(rfm9x->reset_spi_nss_pin())
//   check(rfm9x->spi_transfer(&com))
//   check(rfm9x->spi_transfer(version))
//   check(rfm9x->set_spi_nss_pin())

//   return RFM9X_RESULT_OK;
// }

void RFM9X_Init(const rfm9x_t* const rfm9x) {
  rfm9x->set_reset_pin();
  rfm9x->delay(15);
}

void RFM9X_Reset(const rfm9x_t* const rfm9x) {
  rfm9x->reset_reset_pin();
  rfm9x->delay(15);

  RFM9X_Init(rfm9x);
}

// rfm9x_result_t RFM9X_GetFrequency(const rfm9x_t* const rfm9x, uint32_t* const frequencyHz) {
//   uint8_t com = RFM9X_READ | RFM9X_REG_FREQUENCY;
//   uint8_t val;

//   *frequencyHz = 0;
//   check(rfm9x->reset_spi_nss_pin())
//   check(rfm9x->spi_transfer(&com))
//   check(rfm9x->spi_transfer(&val))
//   *frequencyHz += val << 16;
//   check(rfm9x->spi_transfer(&val))
//   *frequencyHz += val << 8;
//   check(rfm9x->spi_transfer(&val))
//   *frequencyHz += val;
//   check(rfm9x->set_spi_nss_pin())
//   *frequencyHz *= 61;

//   return RFM9X_RESULT_OK;
// }

// rfm9x_result_t RFM9X_GetSyncWord(const rfm9x_t* const rfm9x, uint8_t syncWord[8], uint8_t* const length) {
//   uint8_t com = RFM9X_READ | RFM9X_REG_SYNC_CONFIG;
//   uint8_t syncConfig;

//   check(rfm9x->reset_spi_nss_pin())
//   check(rfm9x->spi_transfer(&com))
//   check(rfm9x->spi_transfer(&syncConfig))
  
//   for (uint8_t cntr = 0; cntr < 8; cntr++) {
//     check(rfm9x->spi_transfer(syncWord + cntr))
//   }
  
//   check(rfm9x->set_spi_nss_pin())
//   *length = (syncConfig & 0b00000111) + 1;

//   return RFM9X_RESULT_OK;
// }

void RFM9X_SetSyncWord(const rfm9x_t* const rfm9x, const uint8_t* const syncWord, uint8_t length) {
  uint8_t d[] = {RFM9X_READ | RFM9X_REG_SYNC_CONFIG, 0x00};

  rfm9x->spi_transfer(d, 2);

  uint8_t syncConfig = d[1];

  uint8_t l = length;
  l -= 1;
  l &= 0b00000111;
  syncConfig &= 0b11111000;
  syncConfig |= l;

  uint8_t d2[length + 2];

  d2[0] = RFM9X_WRITE | RFM9X_REG_SYNC_CONFIG;
  d2[1] = syncConfig;

  memcpy((d2 + 2), syncWord, length);

  rfm9x->spi_transfer(d2, length + 2);
}

// rfm9x_result_t RFM9X_GetMode(const rfm9x_t* const rfm9x, rfm9x_mode_t* const mode) {
//   uint8_t com = RFM9X_READ | RFM9X_REG_MODE;

//   check(rfm9x->reset_spi_nss_pin())
//   check(rfm9x->spi_transfer(&com))
//   check(rfm9x->spi_transfer((uint8_t *)mode))
//   check(rfm9x->set_spi_nss_pin())

//   *mode = (rfm9x_mode_t)(*mode & 0b00000111);

//   return RFM9X_RESULT_OK;
// }

void RFM9X_SetMode(const rfm9x_t* const rfm9x, const rfm9x_mode_t* const mode) {
  uint8_t d[] = {RFM9X_READ | RFM9X_REG_MODE, 0x00};

  rfm9x->spi_transfer(d, 2);

  uint8_t oldMode = d[1];

  oldMode &= 0b11111000;
  oldMode |= *mode;

  d[0] = RFM9X_WRITE | RFM9X_REG_MODE; 
  d[1] = oldMode;

  rfm9x->spi_transfer(d, 2);
}

void RFM9X_GetFlags(const rfm9x_t* const rfm9x, uint16_t* const flags) {
  uint8_t d[] = {RFM9X_READ | RFM9X_REG_IRQ_FLAGS, 0x00, 0x00};

  *flags = 0;

  rfm9x->spi_transfer(d, 3);

  *flags += ((uint16_t)d[1] << 8);
  *flags += d[2];
}

// rfm9x_result_t RFM9X_SetCrcAutoClearMode(const rfm9x_t* const rfm9x, const rfm9x_crc_autoclear_mode_t* const mode) {
//   uint8_t com = RFM9X_READ | RFM9X_REG_PACKET_CONFIG_1;
//   uint8_t oldMode = 0;

//   check(rfm9x->reset_spi_nss_pin())
//   check(rfm9x->spi_transfer(&com))
//   check(rfm9x->spi_transfer(&oldMode))
//   check(rfm9x->set_spi_nss_pin())

//   if (*mode == RFM9X_CRC_AUTOCLEAR_OFF) {
//     oldMode |= RFM9X_CRC_AUTOCLEAR_OFF;
//   } else {
//     oldMode &= RFM9X_CRC_AUTOCLEAR_ON;
//   }
//   com = RFM9X_WRITE | RFM9X_REG_PACKET_CONFIG_1;

//   check(rfm9x->reset_spi_nss_pin())
//   check(rfm9x->spi_transfer(&com))
//   check(rfm9x->spi_transfer(&oldMode))
//   check(rfm9x->set_spi_nss_pin())

//   return RFM9X_RESULT_OK;
// }

// rfm9x_result_t RFM9X_SetFreqDev(const rfm9x_t* const rfm9x, const rfm9x_freq_dev_t* const freqDev) {
//   uint8_t com = RFM9X_WRITE | RFM9X_REG_FDEV_MSB;
//   uint16_t tmp = (uint16_t)(*freqDev) & 0x3FFF;
//   uint8_t spi_tmp = tmp >> 8;

//   check(rfm9x->reset_spi_nss_pin())
//   check(rfm9x->spi_transfer(&com))
//   check(rfm9x->spi_transfer(&spi_tmp))
//   spi_tmp = tmp;
//   check(rfm9x->spi_transfer(&spi_tmp))
//   check(rfm9x->set_spi_nss_pin())

//   return RFM9X_RESULT_OK;
// }

void RFM9X_SetPower(const rfm9x_t* const rfm9x, const uint8_t* const outputPower) {
  uint8_t d[] = {RFM9X_WRITE | RFM9X_REG_PA_CONFIG, 0x8F};

  // if (*outputPower > 2) {
  //   d[1] |= (*outputPower - 2) & 0x0f;
  // }

  rfm9x->spi_transfer(d, 2);
}

// rfm9x_result_t RFM9X_GetPower(const rfm9x_t* const rfm9x, uint8_t* const outputPower) {
//   uint8_t com = RFM9X_READ | RFM9X_REG_PA_CONFIG;
//   uint8_t flagByte;

//   check(rfm9x->reset_spi_nss_pin())
//   check(rfm9x->spi_transfer(&com))
//   check(rfm9x->spi_transfer(&flagByte))
//   check(rfm9x->set_spi_nss_pin())

//   *outputPower = (flagByte & 0x0f) + 2;

//   return RFM9X_RESULT_OK;
// }

// rfm9x_result_t RFM9X_GetRSSIValue(const rfm9x_t* const rfm9x, uint8_t* const rssiValue) {
//   uint8_t com = RFM9X_READ | RFM9X_REG_RSSI_VALUE;

//   check(rfm9x->reset_spi_nss_pin())
//   check(rfm9x->spi_transfer(&com))
//   check(rfm9x->spi_transfer(rssiValue))
//   check(rfm9x->set_spi_nss_pin())

//   return RFM9X_RESULT_OK;
// }
