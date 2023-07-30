/*
 * uart.c
 *
 *  Created on: Jul 24, 2023
 *      Author: KIMMINJI
 */

#include "uart.h"
#include "qbuffer.h"



typedef struct
{
  uint32_t baudrate;

  uint32_t databit;              /*!< Specifies the number of data bits transmitted or received in a frame.
                                         This parameter can be a value of @ref UARTEx_Word_Length. */

  uint32_t stopbit;                /*!< Specifies the number of stop bits transmitted.
                                         This parameter can be a value of @ref UART_Stop_Bits. */

  uint32_t parity;

  uint32_t flowCtl;               /*!< Specifies whether the hardware flow control mode is enabled
                                           or disabled.
                                           This parameter can be a value of @ref UART_Hardware_Flow_Control. */
} uart_tbl_t;


uart_tbl_t uart_tbl[UART_MAX_CH] =
      {
          {19200, UART_WORDLENGTH_8B, UART_STOPBITS_1, UART_PARITY_NONE, UART_HWCONTROL_NONE},
          {19200, UART_WORDLENGTH_8B, UART_STOPBITS_1, UART_PARITY_NONE, UART_HWCONTROL_NONE},
      };


static bool is_open[UART_MAX_CH];

static qbuffer_t qbuffer[UART_MAX_CH];
static uint8_t rx_buf[256];

extern uint8_t RxData[256];

bool available_flag = false;

UART_HandleTypeDef huart1; //uart1: Debugging
UART_HandleTypeDef huart2; //uart2: RS485 communication
DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef hdma_usart2_rx;


bool uartInit(void)
{
  for (int i=0; i<UART_MAX_CH; i++)
    {
      is_open[i] = false;
    }
  return true;
}

bool uartOpen(uint8_t ch)
{
  bool ret = false;


  switch(ch)
  {
    case _DEF_UART1:


      huart1.Instance                    = USART1;
      huart1.Init.Mode                   = UART_MODE_TX_RX;
      huart1.Init.OverSampling           = UART_OVERSAMPLING_16;
      huart1.Init.OneBitSampling         = UART_ONE_BIT_SAMPLE_DISABLE;
      huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

      huart1.Init.BaudRate               = uart_tbl[ch].baudrate;
      huart1.Init.WordLength             = uart_tbl[ch].databit;
      huart1.Init.StopBits               = uart_tbl[ch].stopbit;
      huart1.Init.Parity                 = uart_tbl[ch].parity;
      huart1.Init.HwFlowCtl              = uart_tbl[ch].flowCtl;

      qbufferCreate(&qbuffer[ch], &rx_buf[0], 256);

      HAL_UART_DeInit(&huart1);

      __HAL_RCC_DMA1_CLK_ENABLE();
      HAL_NVIC_SetPriority(DMA1_Channel2_3_IRQn, 0, 0);
      HAL_NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);

      if (HAL_UART_Init(&huart1) != HAL_OK)
      {
        ret = false;
      }
      else
      {
        ret = true;
        is_open[ch] = true;

         /*DMA*/
        if(HAL_UARTEx_ReceiveToIdle_DMA(&huart1, (uint8_t *)&rx_buf[0], 256) != HAL_OK)
          {
            ret = false;
          }
        __HAL_DMA_DISABLE_IT(&hdma_usart1_rx, DMA_IT_HT);

        qbuffer[ch].in = 0;
        qbuffer[ch].out = 0;
      }
      break;

    case _DEF_UART2:
      huart2.Instance                    = USART2;
      huart2.Init.Mode                   = UART_MODE_TX_RX;
      huart2.Init.OverSampling           = UART_OVERSAMPLING_16;
      huart2.Init.OneBitSampling         = UART_ONE_BIT_SAMPLE_DISABLE;
      huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

      huart2.Init.BaudRate               = uart_tbl[ch].baudrate;
      huart2.Init.WordLength             = uart_tbl[ch].databit;
      huart2.Init.StopBits               = uart_tbl[ch].stopbit;
      huart2.Init.Parity                 = uart_tbl[ch].parity;
      huart2.Init.HwFlowCtl              = uart_tbl[ch].flowCtl;

      qbufferCreate(&qbuffer[ch], &rx_buf[0], 256);

      HAL_UART_DeInit(&huart2);

      __HAL_RCC_DMA1_CLK_ENABLE();
      HAL_NVIC_SetPriority(DMA1_Channel4_5_IRQn, 0, 0);
      HAL_NVIC_EnableIRQ(DMA1_Channel4_5_IRQn);

      if (HAL_UART_Init(&huart2) != HAL_OK)
      {
        ret = false;
      }
      else
      {
        ret = true;
        is_open[ch] = true;

         /*DMA*/
        if(HAL_UARTEx_ReceiveToIdle_DMA(&huart2, (uint8_t *)&rx_buf[0], 256) != HAL_OK)
          {
            ret = false;
          }
        __HAL_DMA_DISABLE_IT(&hdma_usart2_rx, DMA_IT_HT);

        qbuffer[ch].in = 0;
        qbuffer[ch].out = 0;
      }
      break;

  }

  return ret;
}

uint32_t uartAvailable(uint8_t ch)
{
  uint32_t ret = 0;
  if(available_flag)
  {
    switch(ch)
    {
      case _DEF_UART1:
          ret= qbufferAvailable(&qbuffer[ch]);
          if(ret == 0)
          {
            available_flag = false;
          }
        break;
      case _DEF_UART2:
          ret= qbufferAvailable(&qbuffer[ch]);
          if(ret == 0)
          {
            available_flag = false;
          }
        break;
    }
  }
  return ret;
}

uint8_t uartRead(uint8_t ch)
{
  uint8_t ret = 0;

  switch(ch)
  {
    case _DEF_UART1:
      qbufferRead(&qbuffer[_DEF_UART1], RxData, 256);
      break;
    case _DEF_UART2:
      qbufferRead(&qbuffer[_DEF_UART2], RxData, 256);
      break;
  }

  return ret;
}

uint32_t uartWrite(uint8_t ch, uint8_t *p_data, uint32_t length)
{
  uint32_t ret = 0;
  HAL_StatusTypeDef status;

  switch(ch)
  {
    case _DEF_UART1:
      status = HAL_UART_Transmit(&huart1, p_data, length, 100);
      if (status == HAL_OK)
      {
        ret = length;
      }
      break;
    case _DEF_UART2:
      status = HAL_UART_Transmit(&huart2, p_data, length, 100);
      if (status == HAL_OK)
      {
        ret = length;
      }
      break;
  }
  return ret;
}

uint32_t uartPrintf(uint8_t ch, char *fmt, ...)
{
  char buf[256];
  va_list args;
  int len;
  uint32_t ret;

  va_start(args, fmt);

  len = vsnprintf(buf, 256, fmt, args);

  ret = uartWrite(ch, (uint8_t *)buf, len);

  va_end(args);

  return ret;
}


/*
 * uart_tbl_mem_tag
 * 0 : baudrate
 * 1 : databit
 * 2 : stopbit
 * 3 : parity
 * 4 : flowCtl
 */
bool set_uart_tbl(uint8_t ch, uint16_t* Holding_Registers_Database)
{
  bool ret = true;


  is_open[ch] = false;

  for (int i=0; i<5; i++)
  {
    int uart_tbl_mem_tag = i;

    switch (uart_tbl_mem_tag)
    {
      case 0:
        uart_tbl[ch].baudrate = (uint32_t)Holding_Registers_Database[uart_tbl_mem_tag+1]<<8 | (uint32_t)Holding_Registers_Database[uart_tbl_mem_tag+2]<<0;
        break;

      case 1:
        if(Holding_Registers_Database[uart_tbl_mem_tag+2] == 0x08)
        {
          uart_tbl[ch].databit = (uint32_t)UART_WORDLENGTH_8B;
        }
        break;

      case 2:
        if(Holding_Registers_Database[uart_tbl_mem_tag+2] == 0x01)
        {
          uart_tbl[ch].stopbit = (uint32_t)UART_STOPBITS_1;
        }
        break;

      case 3:
        if(Holding_Registers_Database[uart_tbl_mem_tag+2] == 0x00)
        {
          uart_tbl[ch].parity = (uint32_t)UART_PARITY_NONE;
        }
        break;

      case 4:
        if((uint32_t)Holding_Registers_Database[uart_tbl_mem_tag+2] == 0x00)
        {
          uart_tbl[ch].flowCtl = (uint32_t)UART_HWCONTROL_NONE;
        }
        break;

      default:
        ret = false;
        break;
    }
  }

    return ret;
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
  if (huart->Instance == USART1)
    {

    }
  if (huart->Instance == USART2)
    {

    }
}


void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
  bool ret = true;

  if (huart->Instance == USART1) //case _DEF_UART1
  {
    qbuffer[_DEF_UART1].in = 0;
    qbuffer[_DEF_UART1].out = 0;

    if (Size > 256 - qbuffer[_DEF_UART1].in)
    {
           // 처리할 수 없는 크기의 데이터가 수신됨
           // 예외 처리 또는 에러 처리를 수행
      return;
    }
    qbuffer[_DEF_UART1].in = qbuffer[_DEF_UART1].len - hdma_usart1_rx.Instance->CNDTR;
    available_flag = true;

    HAL_UART_DeInit(&huart1);
    if (HAL_UART_Init(&huart1) != HAL_OK)
    {
      ret = false;
    }
    else
    {
      ret = true;
      is_open[_DEF_UART1] = true;


      if(HAL_UARTEx_ReceiveToIdle_DMA(&huart1, (uint8_t *)&rx_buf[0], 256) != HAL_OK)
      {
        ret = false;
      }
      __HAL_DMA_DISABLE_IT(&hdma_usart1_rx, DMA_IT_HT);
      }

  }
  else if (huart->Instance == USART2) //case _DEF_UART2
  {

    qbuffer[_DEF_UART2].in = 0;
    qbuffer[_DEF_UART2].out = 0;

    if (Size > 256 - qbuffer[_DEF_UART2].in)
    {
           // 처리할 수 없는 크기의 데이터가 수신됨
           // 예외 처리 또는 에러 처리를 수행
      return;
    }
    qbuffer[_DEF_UART2].in = qbuffer[_DEF_UART2].len - hdma_usart2_rx.Instance->CNDTR;
    available_flag = true;

    HAL_UART_DeInit(&huart2);
    if (HAL_UART_Init(&huart2) != HAL_OK)
    {
      ret = false;
    }
    else
    {
      ret = true;
      is_open[_DEF_UART2] = true;


      if(HAL_UARTEx_ReceiveToIdle_DMA(&huart2, (uint8_t *)&rx_buf[0], 256) != HAL_OK)
      {
        ret = false;
      }
      __HAL_DMA_DISABLE_IT(&hdma_usart2_rx, DMA_IT_HT);
      }

  }

}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
#if 0
  if (huart->Instance == USART1)
  {
    qbufferWrite(&qbuffer[_DEF_UART1], &rx_data[_DEF_UART1], 1);

    HAL_UART_Receive_IT(&huart1, (uint8_t *)&rx_data[_DEF_UART1], 1);
  }
#endif
}



void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */
    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF1_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART1 DMA Init */
    /* USART1_RX Init */
    hdma_usart1_rx.Instance = DMA1_Channel3;
    hdma_usart1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_usart1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart1_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart1_rx.Init.Mode = DMA_CIRCULAR;
    hdma_usart1_rx.Init.Priority = DMA_PRIORITY_LOW;
    if (HAL_DMA_Init(&hdma_usart1_rx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmarx,hdma_usart1_rx);

    /* USART1 interrupt Init */
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  }
  else if(uartHandle->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspInit 0 */

  /* USER CODE END USART2_MspInit 0 */
    /* USART2 clock enable */
    __HAL_RCC_USART2_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART2 GPIO Configuration
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF1_USART2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART2 DMA Init */
    /* USART2_RX Init */
    hdma_usart2_rx.Instance = DMA1_Channel5;
    hdma_usart2_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_usart2_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart2_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart2_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart2_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart2_rx.Init.Mode = DMA_CIRCULAR;
    hdma_usart2_rx.Init.Priority = DMA_PRIORITY_LOW;
    if (HAL_DMA_Init(&hdma_usart2_rx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmarx,hdma_usart2_rx);

    /* USART2 interrupt Init */
    HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART2_IRQn);
  /* USER CODE BEGIN USART2_MspInit 1 */

  /* USER CODE END USART2_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();

    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);

    /* USART1 DMA DeInit */
    HAL_DMA_DeInit(uartHandle->hdmarx);

    /* USART1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }
  else if(uartHandle->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspDeInit 0 */

  /* USER CODE END USART2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART2_CLK_DISABLE();

    /**USART2 GPIO Configuration
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2|GPIO_PIN_3);

    /* USART2 DMA DeInit */
    HAL_DMA_DeInit(uartHandle->hdmarx);

    /* USART2 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART2_IRQn);
  /* USER CODE BEGIN USART2_MspDeInit 1 */

  /* USER CODE END USART2_MspDeInit 1 */
  }
}
