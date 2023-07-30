/*
 * bsp.h
 *
 *  Created on: Jul 24, 2023
 *      Author: KIMMINJI
 */

#ifndef SRC_BSP_BSP_H_
#define SRC_BSP_BSP_H_

#include "def.h"

#include "stm32f0xx_hal.h"

void bspInit(void);

void delay(uint32_t ms);
uint32_t millis(void);

void Error_Handler(void);

#endif /* SRC_BSP_BSP_H_ */
