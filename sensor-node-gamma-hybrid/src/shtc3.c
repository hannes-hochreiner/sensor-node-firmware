#include "shtc3.h"

#define SHTC3_CRC_POLY 0x31
#define SHTC3_CRC_INIT 0xFF

result_t shtc3_get_data(const shtc3_t*const shtc3, shtc3_data_t*const data) {
  uint8_t com_wakeup[] = {0x35, 0x17};
  if (shtc3->i2c_write(shtc3->address, com_wakeup, 2) == RESULT_ERROR) {
    // the sensor needs some time to wake up (~.5 ms), so it's ok to get a NACK on the wakeup request
    shtc3->delay(1);
  }

  uint8_t com_id[] = {0xEF, 0xC8};
  if (shtc3->i2c_write(shtc3->address, com_id, 2) == RESULT_ERROR) {
    return RESULT_ERROR;
  }

  uint8_t data_id[3];
  if (shtc3->i2c_read(shtc3->address, data_id, 3) == RESULT_ERROR) {
    return RESULT_ERROR;
  }

  uint8_t crc_id;

  shtc3->crc_8(SHTC3_CRC_POLY, SHTC3_CRC_INIT, data_id, 2, &crc_id);

  if (crc_id != data_id[2]) {
    return RESULT_ERROR;
  }
  data->id = (((uint16_t)data_id[0]) << 8) + data_id[1];

  // uint8_t com_meas[] = {0x7C, 0xA2}; // TEMP first
  uint8_t com_meas[] = {0x5C, 0x24}; // HUM first
  if (shtc3->i2c_write(shtc3->address, com_meas, 2) == RESULT_ERROR) {
    return RESULT_ERROR;
  }

  uint8_t data_meas[6];
  if (shtc3->i2c_read(shtc3->address, data_meas, 6) == RESULT_ERROR) {
    return RESULT_ERROR;
  }

  uint8_t com_sleep[] = {0xB0, 0x98};
  if (shtc3->i2c_write(shtc3->address, com_sleep, 2) == RESULT_ERROR) {
    return RESULT_ERROR;
  }

  uint8_t crc_temp;
  uint8_t crc_hum;

  shtc3->crc_8(SHTC3_CRC_POLY, SHTC3_CRC_INIT, data_meas, 2, &crc_hum);
  shtc3->crc_8(SHTC3_CRC_POLY, SHTC3_CRC_INIT, (data_meas + 3), 2, &crc_temp);

  if (crc_temp != data_meas[5]) {
    return RESULT_ERROR;
  }
  data->temperature = ((float)data_meas[3] / 256 + (float)data_meas[4] / 65536) * 175 - 45;

  if (crc_hum != data_meas[2]) {
    return RESULT_ERROR;
  }
  data->humidity = ((float)data_meas[0] / 256 + (float)data_meas[1] / 65536) * 100;

  return RESULT_OK;
}
