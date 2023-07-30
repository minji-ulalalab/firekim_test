/*
 * button.h
 *
 *  Created on: Jul 24, 2023
 *      Author: KIMMINJI
 */

#ifndef SRC_COMMON_HW_INCLUDE_BUTTON_H_
#define SRC_COMMON_HW_INCLUDE_BUTTON_H_

#include "hw_def.h"


#ifdef _USE_HW_BUTTON

#define BUTTON_MAX_CH        HW_BUTTON_MAX_CH

bool buttonInit(void);
GPIO_PinState buttonStateCheck(uint8_t ch);

#endif


#endif /* SRC_COMMON_HW_INCLUDE_BUTTON_H_ */
