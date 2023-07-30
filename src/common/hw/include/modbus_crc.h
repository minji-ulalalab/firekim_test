/*
 * modbus_crc.h
 *
 *  Created on: Jul 24, 2023
 *      Author: KIMMINJI
 */

#ifndef SRC_COMMON_HW_INCLUDE_MODBUS_CRC_H_
#define SRC_COMMON_HW_INCLUDE_MODBUS_CRC_H_

#include "stdint.h"

uint16_t crc16(uint8_t *buffer, uint16_t buffer_length);

#endif /* SRC_COMMON_HW_INCLUDE_MODBUS_CRC_H_ */
