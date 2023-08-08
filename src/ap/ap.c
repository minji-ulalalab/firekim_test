/*
 * ap.c
 *     version: v1.100
 *  Created on: Jul 24, 2023
 *      Author: KIMMINJI
 */


#include "ap.h"

#define FLASH_BUF_SIZE      20

typedef enum {
    INIT_STATE,
    MODE_STATE,
    MODBUS_DATA_STATE,
    CLE_STATE
} State;



State ap_State;
uint32_t ap_time;

uint8_t reset_flag = false;

/*
 * Database: Holding_Registers_Database
 * Function Code: 3, 6
 * Read/Write Possible
 * -------------------------------
 * | Addr |  Size  |  Description  |
 * -------------------------------
 * |  00  | UINT16 |  Slave    ID  |
 * |  01  | UINT16 |  Baudrate_HH  |
 * |  02  | UINT16 |  Baudrate_LL  |
 * |  03  | UINT16 |  Data    Bit  |
 * |  04  | UINT16 |  Stop    Bit  |
 * |  05  | UINT16 |  Parity  Bit  |
 * |  06  | UINT16 |  Flow Control |
 * |  07  | UINT16 |  CLI    FLAG  |
 * ---------------------------------
 * */

static uint16_t Holding_Registers_Database[50]={
    0x0001,  0x0000,  0x4B00,  0x0008,  0x0001,  0x0000,  0x0000,  0x0000,  0x0000,  0x0000,    // 0-9   40001-40010
    0x0000,  0x0000,  0x0000,  0x0000,  0x0000,  0x0000,  0x0000,  0x0000,  0x0000,  0x0000,   // 10-19 40011-40020
    0x0000,  0x0000,  0x0000,  0x0000,  0x0000,  0x0000,  0x0000,  0x0000,  0x0000,  0x0000,   // 20-29 40021-40030
    0x0000,  0x0000,  0x0000,  0x0000,  0x0000,  0x0000,  0x0000,  0x0000,  0x0000,  0x0000,   // 30-39 40031-40040
    0x0000,  0x0000,  0x0000,  0x0000,  0x0000,  0x0000,  0x0000,  0x0000,  0x0000,  0x0000,   // 40-49 40041-40050
};

/*
 * Database: Input_Registers_Database
 * Function Code: 4
 * Read Only Possible
 * -----------------------------------
 * | Addr |  Size  |    Description   |
 * -----------------------------------
 * |  00  | UINT16 |  Reed  Input     |
 * |  01  | UINT16 |  Operation Time  |
 * |  02  | UINT16 |  Last Err Code   |
 * -----------------------------------
 * */

static uint16_t Input_Registers_Database[50]={
    0x0000,  0x0000,  0x0000,  0x0000,  0x0000,  0x0000,  0x0000,  0x0000,  0x0000,  0x0000,   // 0-9   30001-30010
    0x0000,  0x0000,  0x0000,  0x0000,  0x0000,  0x0000,  0x0000,  0x0000,  0x0000,  0x0000,   // 10-19 30011-30020
    0x0000,  0x0000,  0x0000,  0x0000,  0x0000,  0x0000,  0x0000,  0x0000,  0x0000,  0x0000,   // 20-29 30021-30030
    0x0000,  0x0000,  0x0000,  0x0000,  0x0000,  0x0000,  0x0000,  0x0000,  0x0000,  0x0000,   // 30-39 30031-30040
    0x0000,  0x0000,  0x0000,  0x0000,  0x0000,  0x0000,  0x0000,  0x0000,  0x0000,  0x0000,   // 40-49 30041-30050
};

extern uint8_t RxData[256];

bool Update_Flash_From_Modbus_MAP(uint8_t *flash_data, uint8_t size);
bool Update_Modbus_MAP_From_Flash(uint8_t *info_data, uint8_t size);



void apInit(void)
{
  uint8_t cli_flag_buf[2];
  uint8_t buf[FLASH_BUF_SIZE];


  uartOpen(_DEF_UART1); //debugging port PA9|PA10
  uartOpen(_DEF_UART2); //rs485 communicaation port PA2|PA3
  uartPrintf(_DEF_UART1, "_DEF_UART1 OK..\r\n");
  uartPrintf(_DEF_UART2, "_DEF_UART2 OK..\r\n");

  //Read cli Flag
  flashRead(_CLI_MODE_FLASH_ADDR, cli_flag_buf, 2);
  //for test >> reset
  //cli_flag_buf[0] = 0x00;
  //cli_flag_buf[1] = 0x00;
  /*If CLI FLAG == 0xffff,
  * Update ModbusMap from Flash*/

  if (((uint16_t)cli_flag_buf[0] << 8 | cli_flag_buf[1]) == 0xffff)
  {
    Update_Modbus_MAP_From_Flash(buf, FLASH_BUF_SIZE);
  }
  /*If CLI FLAG == 0x0000,
   * Update Flash from ModbusMap(initial value)*/
  else
  {
    Update_Flash_From_Modbus_MAP(buf, FLASH_BUF_SIZE);
  }
}

void apMain(void)
{

  ap_time = millis();
  ap_State = INIT_STATE;
  while(1)
  {
     switch (ap_State)
     {
                 case INIT_STATE:
                   InitStateHandler();
                     break;

                 case MODE_STATE:
                   ModeStateHandler(ap_time);
                     break;

                 case MODBUS_DATA_STATE:
                   ModbusDataStateHandler();
                     break;

                 case CLE_STATE:
                   CLEStateHandler();
                     break;

                 default:
                     printf("please check your State.\n");
                     break;
     }
   }
}

uint8_t Read_Reed_state(void)
{
  uint8_t ret = 0;
  GPIO_PinState state;

  state = buttonStateCheck(_DEF_BUTTON2);
  /*
   * GPIO_PIN_SET = 1
   * GPIO_PIN_RESET = 0
   */
  if (state == GPIO_PIN_SET)
  {
    /*push button*/
    ret = 1;
    /*set the database to 0xffff*/
    Input_Registers_Database[0] = 0xffff;
    ledToggle(_DEF_LED1);
    ledToggle(_DEF_LED2);

  }
  else
  {
    ret = 0;
    /*set the database to 0x1234*/
    Input_Registers_Database[0] = 0x0000;
    ledOff(_DEF_LED1);
    ledOff(_DEF_LED2);
  }

  return ret;
}
bool Update_Flash_From_Modbus_MAP(uint8_t *flash_data, uint8_t size)
{
  bool ret = false;

  if (flashErase(_MODBUS_FLASH_ADDR, FLASH_BUF_SIZE) == true)
  {
    uartPrintf(_DEF_UART1, "OK...\r\n");
    for(int i=0; i<10; i++)
    {
      flash_data[i*2] = (Holding_Registers_Database[i]>>8) & 0xff;
      flash_data[(i*2)+1] = (Holding_Registers_Database[i]) & 0xff;

      if (i == 9)
      {
        flash_data[(i*2)+2] = (Holding_Registers_Database[i+1]>>8) & 0xff;
      }
    }
    uartPrintf(_DEF_UART1, "Write..\r\n");

    if (flashWrite(_MODBUS_FLASH_ADDR, flash_data, FLASH_BUF_SIZE) == true)
    {
      uartPrintf(_DEF_UART1, "Write OK..\r\n");
    }
    else
    {
      uartPrintf(_DEF_UART1, "Write Fail..\r\n");
    }
  }
  else
  {
    uartPrintf(_DEF_UART1, "Fail...\r\n");
  }
  return ret;
}


bool Update_Modbus_MAP_From_Flash(uint8_t *info_data, uint8_t size)
{
  bool ret = false;

  uartPrintf(_DEF_UART1, "Flash Read...\r\n");
  flashRead(_MODBUS_FLASH_ADDR, info_data, size);

  for (int i=0; i<size; i++)
  {
    Holding_Registers_Database[i] =info_data[i*2]<< 8;
    Holding_Registers_Database[i] |= info_data[i*2+1] << 0;
  }

  return ret;
}

/*Event Handler*/

void InitStateHandler(void)
{
  uint8_t buf[FLASH_BUF_SIZE];

  printf("STATE.INIT_STATE\n");

  /*update from CLI*/
  if(reset_flag)
  {
    Update_Flash_From_Modbus_MAP(buf, FLASH_BUF_SIZE);
    reset_flag = false;
  }
  set_uart_tbl(_DEF_UART2, Holding_Registers_Database);
  /*uart open*/
  uartOpen(_DEF_UART2);

  ap_State = MODE_STATE;
}


void ModeStateHandler(uint32_t pre_time)
{

  printf("STATE.SELECT_MODE_STATE\n");

  if (millis()-pre_time >= 500)
  {
    ap_time = millis();
    Read_Reed_state();
    uartPrintf(_DEF_UART1, "Reed : 0x%X\r\n", Input_Registers_Database[0]);
  }
  ap_State = MODBUS_DATA_STATE;
}


void ModbusDataStateHandler(void)
{
  uint8_t ret = 0;
  printf("STATE.MODBUS_DATA_STATE\n");

  uint16_t slave_id = Holding_Registers_Database[0];

  /*the port get rx_data -> modbus protocol start point*/

  if (uartAvailable(_DEF_UART2)>0)
  {
    uartRead(_DEF_UART2);
    uartPrintf(_DEF_UART1, "Modbus Start\r\n");
    if (RxData[0] == slave_id)
    {
      switch (RxData[1])
      {
        case 0x03:
          readHoldingRegs(Holding_Registers_Database);
          break;
        case 0x04:
          readInputRegs(Input_Registers_Database);
          break;
        case 0x06:
         ret = writeSingleReg(Holding_Registers_Database);
          break;
        default:
          modbusException(ILLEGAL_FUNCTION);
          break;
      }
    }
    else
    {
      modbusException(SLAVE_DEVICE_FAILURE);
    }
  }

  if (ret == 2)
  {
    ap_State = CLE_STATE;
  }
  else
  {
    ap_State = MODE_STATE;//MODE_STATE
  }

}

void CLEStateHandler(void)
{
  cliOpen(_DEF_UART2);  //debugging port
  cliMain(Holding_Registers_Database);

  reset_flag = true;

  ap_State = INIT_STATE;
}

