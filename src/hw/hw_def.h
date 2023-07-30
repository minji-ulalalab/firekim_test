/*
 * hw_def.h
 *
 *  Created on: Jul 24, 2023
 *      Author: KIMMINJI
 */

#ifndef SRC_HW_HW_DEF_H_
#define SRC_HW_HW_DEF_H_

#include "def.h"
#include "bsp.h"


#define _USE_HW_LED
#define      HW_LED_MAX_CH        3

#define _USE_HW_UART
#define      HW_UART_MAX_CH       2

#define _USE_HW_BUTTON
#define      HW_BUTTON_MAX_CH     1

#define _USE_HW_FLASH

#define _USE_HW_CLI
#define      HW_CLI_CMD_NAME_MAX    16
#define      HW_CLI_CMD_LIST_MAX    16
#define      HW_CLI_LINE_HIS_MAX  4
#define      HW_CLI_LINE_BUF_MAX  32

#define HW_SLAVE_ID               1

#define HW_ILLEGAL_FUNCTION       0x01
#define HW_ILLEGAL_DATA_ADDRESS   0x02
#define HW_ILLEGAL_DATA_VALUE     0x03

#define _MODBUS_FLASH_ADDR       0x0800F000
#define _SLAVE_ID_FLASH_ADDR     0x800F000              //size -> uint16_t(2byte)
#define _BAUDRATE_HH_FLASH_ADDR  0x800F000 + 0x00000002 //size -> uint16_t(2byte)
#define _BAUDRATE_LL_FLASH_ADDR  0x800F000 + 0x00000004 //size -> uint16_t(2byte)
#define _DATABIT_FLASH_ADDR      0x800F000 + 0x00000006 //size -> uint16_t(2byte)
#define _PARITY_FLASH_ADDR       0x800F000 + 0x00000008 //size -> uint16_t(2byte)
#define _STOPBIT_FLASH_ADDR      0x800F000 + 0x0000000A //size -> uint16_t(2byte)
#define _FLOWCTL_FLASH_ADDR      0x800F000 + 0x0000000C //size -> uint16_t(2byte)
#define _CLI_MODE_FLASH_ADDR     0x800F000 + 0x0000000E //size -> uint16_t(2byte)


#endif /* SRC_HW_HW_DEF_H_ */
