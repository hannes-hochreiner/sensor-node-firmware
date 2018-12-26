#include "shtc3.h"

static const uint8_t shtc3_address = 0x70;
static const uint8_t shtc3_crc_poly = 0x31;
static const uint8_t shtc3_crc_init = 0xFF;

shtc3_result_t shtc3_get_data(const shtc3_t*const shtc3, shtc3_data_t*const data) {
  uint8_t com_wakeup[] = {0x35, 0x17};
  if (shtc3->i2c_write(shtc3_address, com_wakeup, 2) == RESULT_ERROR) {
    return SHTC3_RESULT_ERROR_I2C_WRITE_WAKEUP;
  }
  
  // give the sensor some time to wake up
  shtc3->delay(1);

  uint8_t com_id[] = {0xEF, 0xC8};
  if (shtc3->i2c_write(shtc3_address, com_id, 2) == RESULT_ERROR) {
    return SHTC3_RESULT_ERROR_I2C_WRITE_ID;
  }

  uint8_t data_id[3];
  if (shtc3->i2c_read(shtc3_address, data_id, 3) == RESULT_ERROR) {
    return SHTC3_RESULT_ERROR_I2C_READ_ID;
  }

  uint8_t crc_id;

  shtc3->crc_8(shtc3_crc_poly, shtc3_crc_init, data_id, 2, &crc_id);

  if (crc_id != data_id[2]) {
    return SHTC3_RESULT_ERROR_CRC;
  }
  data->id = (((uint16_t)data_id[0]) << 8) + data_id[1];

  uint8_t com_meas[] = {0x5C, 0x24}; // HUM first
  if (shtc3->i2c_write(shtc3_address, com_meas, 2) == RESULT_ERROR) {
    return SHTC3_RESULT_ERROR_I2C_WRITE_MEAS;
  }

  uint8_t data_meas[6];
  if (shtc3->i2c_read(shtc3_address, data_meas, 6) == RESULT_ERROR) {
    return SHTC3_RESULT_ERROR_I2C_READ_MEAS;
  }

  uint8_t com_sleep[] = {0xB0, 0x98};
  if (shtc3->i2c_write(shtc3_address, com_sleep, 2) == RESULT_ERROR) {
    return SHTC3_RESULT_ERROR_I2C_WRITE_SLEEP;
  }

  uint8_t crc_temp;
  uint8_t crc_hum;

  shtc3->crc_8(shtc3_crc_poly, shtc3_crc_init, data_meas, 2, &crc_hum);
  shtc3->crc_8(shtc3_crc_poly, shtc3_crc_init, (data_meas + 3), 2, &crc_temp);

  if (crc_temp != data_meas[5]) {
    return SHTC3_RESULT_ERROR_CRC;
  }
  data->temperature = ((float)data_meas[3] / 256 + (float)data_meas[4] / 65536) * 175 - 45;

  if (crc_hum != data_meas[2]) {
    return SHTC3_RESULT_ERROR_CRC;
  }
  data->humidity = ((float)data_meas[0] / 256 + (float)data_meas[1] / 65536) * 100;

  return SHTC3_RESULT_OK;
}
