/*
 * uart.h
 *
 *  Created on: Jul 24, 2023
 *      Author: KIMMINJI
 */

#ifndef SRC_COMMON_HW_INCLUDE_UART_H_
#define SRC_COMMON_HW_INCLUDE_UART_H_

#include "hw_def.h"

#ifdef _USE_HW_UART

#define UART_MAX_CH        HW_UART_MAX_CH


bool     uartInit(void);
bool uartOpen(uint8_t ch);
uint32_t uartAvailable(uint8_t ch);
uint8_t  uartRead(uint8_t ch);
uint32_t uartWrite(uint8_t ch, uint8_t *p_data, uint32_t length);
uint32_t uartPrintf(uint8_t ch, char *fmt, ...);

bool set_uart_tbl(uint8_t ch, uint16_t* Holding_Registers_Database);

#endif


#endif /* SRC_COMMON_HW_INCLUDE_UART_H_ */
