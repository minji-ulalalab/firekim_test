/*
 * cli.h
 *
 *  Created on: 2023. 7. 3.
 *      Author: KIMMINJI
 */

#ifndef SRC_COMMON_HW_INCLUDE_CLI_H_
#define SRC_COMMON_HW_INCLUDE_CLI_H_


#include "hw_def.h"


#ifdef _USE_HW_CLI

#define CLI_CMD_LIST_MAX      HW_CLI_CMD_LIST_MAX
#define CLI_CMD_NAME_MAX      HW_CLI_CMD_NAME_MAX

#define CLI_LINE_BUF_MAX      HW_CLI_LINE_BUF_MAX




bool cliInit(void);
bool cliOpen(uint8_t ch);
bool cliMain(uint16_t* Holding_Registers_Database);
void cliPrintf(const char *fmt, ...);
bool cliKeepLoop(void);


#endif


#endif /* SRC_COMMON_HW_INCLUDE_CLI_H_ */
