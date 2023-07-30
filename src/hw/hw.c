/*
 * hw.c
 *
 *  Created on: Jul 24, 2023
 *      Author: KIMMINJI
 */

#include "hw.h"


#include "hw.h"




void hwInit(void)
{
  bspInit();
  //cliInit();

  ledInit();
  uartInit();
  buttonInit();
  ModbusInit();
  flashInit();

}
