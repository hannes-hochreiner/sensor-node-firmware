#include <stdint.h>
#include <float.h>

typedef struct {
  uint16_t type; // type of the message (always 0x0001)
  uint16_t sensor_id; // id of the sensor
  uint32_t mcu_id_1; // 1st part of the id of the mcu
  uint32_t mcu_id_2; // 2nd part of the id of the mcu
  uint32_t mcu_id_3; // 3rd part of the id of the mcu
  uint32_t message_index; // index of the message (counted up since last reset)
  float temperature; // temperature in degrees celcius
  float humidity; // humidity in %
  uint32_t _rng; // random number as padding
} message_0001_t; // 8 x 32 bit = 8 x 4 byte

typedef struct {
  uint16_t type; // type of the message (always 0x0002)
  uint16_t sensor_id; // id of the sensor
  uint32_t mcu_id_1; // 1st part of the id of the mcu
  uint32_t mcu_id_2; // 2nd part of the id of the mcu
  uint32_t mcu_id_3; // 3rd part of the id of the mcu
  uint32_t message_index; // index of the message (counted up since last reset)
  float temperature; // temperature in degrees celcius
  float humidity; // humidity in %
  float pressure; // pressure in mbar = hPa
} message_0002_t; // 8 x 32 bit = 8 x 4 byte
