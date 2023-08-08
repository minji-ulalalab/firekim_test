/*
 * ap.h
 *
 *  Created on: Jul 24, 2023
 *      Author: KIMMINJI
 */

#ifndef SRC_AP_AP_H_
#define SRC_AP_AP_H_


#include "hw.h"




void apInit(void);
void apMain(void);
uint8_t Read_Reed_state(void);

void InitStateHandler(void);
void ModeStateHandler(uint32_t pre_time);
void ModbusDataStateHandler(void);
void CLEStateHandler(void);

#endif /* SRC_AP_AP_H_ */
