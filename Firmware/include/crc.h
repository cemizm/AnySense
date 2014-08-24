#ifndef CRC_H
#define CRC_H

#include <stdint.h>

uint32_t crc_update(uint32_t crc, const uint8_t *check, int32_t length);

#endif
