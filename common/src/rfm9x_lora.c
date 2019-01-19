#include "rfm9x_lora.h"

const uint8_t RFM9X_READ = 0x00;
const uint8_t RFM9X_WRITE = 0x80;

const uint8_t RFM9X_REG_FIFO = 0x00;
const uint8_t RFM9X_REG_MODE = 0x01;
const uint8_t RFM9X_REG_PA_CONFIG = 0x09;
const uint8_t RFM9X_REG_FIFO_ADDR_PTR = 0x0D;
const uint8_t RFM9X_REG_FIFO_TX_BASE_ADDR = 0x0E;
const uint8_t RFM9X_REG_FIFO_RX_BASE_ADDR = 0x0F;
const uint8_t RFM9X_REG_FIFO_RX_CURRENT_ADDR = 0x10;
const uint8_t RFM9X_REG_IRQ_FLAGS = 0x12;
const uint8_t RFM9X_REG_RX_NB_BYTES = 0x13;
const uint8_t RFM9X_REG_PKT_RSSI = 0x1A;
const uint8_t RFM9X_REG_CONF1 = 0x1D;
const uint8_t RFM9X_REG_CONF2 = 0x1E;
const uint8_t RFM9X_REG_PAYLOAD_LENGTH = 0x22;
const uint8_t RFM9X_REG_CONF3 = 0x26;

void RFM9X_WriteMessage(const rfm9x_t* const rfm9x, uint8_t* const data, uint8_t length) {
  // Set PayloadLength
  uint8_t p[] = {RFM9X_WRITE | RFM9X_REG_PAYLOAD_LENGTH, length};
  rfm9x->spi_transfer(p, 2);

  // Set FifoPtrAddr to FifoTxPtrBase.
  uint8_t f[] = {RFM9X_READ | RFM9X_REG_FIFO_TX_BASE_ADDR, 0x00};
  rfm9x->spi_transfer(f, 2);
  f[0] = RFM9X_WRITE | RFM9X_REG_FIFO_ADDR_PTR;
  rfm9x->spi_transfer(f, 2);

  // Write message to FIFO
  uint8_t d[length + 1];
  d[0] = RFM9X_WRITE | RFM9X_REG_FIFO;
  memcpy((d + 1), data, length);
  rfm9x->spi_transfer(d, length + 1);
}

void RFM9X_ReadMessage(const rfm9x_t* const rfm9x, read_func callback) {
  // Read length
  uint8_t l[] = {RFM9X_READ | RFM9X_REG_RX_NB_BYTES, 0x00};
  rfm9x->spi_transfer(l, 2);

  // set FIFO pointer
  uint8_t a[] = {RFM9X_READ | RFM9X_REG_FIFO_RX_CURRENT_ADDR, 0x00};
  rfm9x->spi_transfer(a, 2);
  a[0] = RFM9X_WRITE | RFM9X_REG_FIFO_ADDR_PTR;
  rfm9x->spi_transfer(a, 2);

  // read package
  uint8_t d[l[1] + 1];

  d[0] = RFM9X_READ | RFM9X_REG_FIFO;

  rfm9x->spi_transfer(d, l[1] + 1);

  callback((d + 1), l[1]);
}

void RFM9X_Init(const rfm9x_t* const rfm9x) {
  rfm9x->set_reset_pin();
  rfm9x->delay(15);
}

void RFM9X_Reset(const rfm9x_t* const rfm9x) {
  rfm9x->reset_reset_pin();
  rfm9x->delay(15);

  RFM9X_Init(rfm9x);
}

void RFM9X_SetMode(const rfm9x_t* const rfm9x, const rfm9x_mode_t* const mode) {
  uint8_t d[] = {RFM9X_WRITE | RFM9X_REG_MODE, *mode};

  rfm9x->spi_transfer(d, 2);
}

void RFM9X_GetMode(const rfm9x_t* const rfm9x, rfm9x_mode_t* const mode) {
  uint8_t d[] = {RFM9X_READ | RFM9X_REG_MODE, 0x00};

  rfm9x->spi_transfer(d, 2);

  *mode = d[1];
}

void RFM9X_SetConf1(const rfm9x_t* const rfm9x, const rfm9x_conf1_t* const conf1) {
  uint8_t d[] = {RFM9X_WRITE | RFM9X_REG_CONF1, *conf1};

  rfm9x->spi_transfer(d, 2);
}

void RFM9X_SetConf2(const rfm9x_t* const rfm9x, const rfm9x_conf2_t* const conf2) {
  uint8_t d[] = {RFM9X_WRITE | RFM9X_REG_CONF2, *conf2};

  rfm9x->spi_transfer(d, 2);
}

void RFM9X_SetConf3(const rfm9x_t* const rfm9x, const rfm9x_conf3_t* const conf3) {
  uint8_t d[] = {RFM9X_WRITE | RFM9X_REG_CONF3, *conf3};

  rfm9x->spi_transfer(d, 2);
}

void RFM9X_GetFlags(const rfm9x_t* const rfm9x, rfm9x_flags_t* const flags) {
  uint8_t d[] = {RFM9X_READ | RFM9X_REG_IRQ_FLAGS, 0x00};

  *flags = 0;

  rfm9x->spi_transfer(d, 2);

  *flags = d[1];
}

void RFM9X_ResetFlags(const rfm9x_t* const rfm9x, const rfm9x_flags_t* const flags) {
  volatile uint8_t fl = *flags;
  uint8_t d[] = {RFM9X_WRITE | RFM9X_REG_IRQ_FLAGS, fl};

  rfm9x->spi_transfer(d, 2);
}

void RFM9X_SetPower(const rfm9x_t* const rfm9x, const uint8_t* const outputPower) {
  uint8_t d[] = {RFM9X_WRITE | RFM9X_REG_PA_CONFIG, 0x80};

  if (*outputPower > 2) {
    d[1] |= (*outputPower - 2) & 0x0f;
  }

  rfm9x->spi_transfer(d, 2);
}

void RFM9X_GetRssiPacket(const rfm9x_t* const rfm9x, int* const rssi) {
  uint8_t d[] = {RFM9X_READ | RFM9X_REG_PKT_RSSI, 0x00};

  rfm9x->spi_transfer(d, 2);
  *rssi = -137 + d[1];
}
