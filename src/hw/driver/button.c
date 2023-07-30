/*
 * button.c
 *
 *  Created on: Jul 24, 2023
 *      Author: KIMMINJI
 */


#include "button.h"



bool buttonInit(void)
{
  bool ret = true;

  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin : PB8 */
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  GPIO_InitStruct.Pin = GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  return ret;

}

GPIO_PinState buttonStateCheck(uint8_t ch)
{
  GPIO_PinState state = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_8);

  return state;
}
