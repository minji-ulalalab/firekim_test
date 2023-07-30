/*
 * modbusSlave.c
 *
 *  Created on: Jul 24, 2023
 *      Author: KIMMINJI
 */


#include "modbusSlave.h"
#include "string.h"

uint8_t RxData[256];
uint8_t TxData[256];
uint8_t buffData[256];


bool ModbusInit(void)
{
  bool ret = false;


  return ret;
}

void sendData (uint8_t *data, int size)
{
  // we will calculate the CRC in this function itself
  uint16_t crc = crc16(data, size);
  data[size] = crc&0xFF;   // CRC LOW
  data[size+1] = (crc>>8)&0xFF;  // CRC HIGH

  uartWrite(_DEF_UART2, data, size+2);
}

bool checkData (uint8_t *data, int size)
{
  bool ret = false;
  // check the CRC
  uint16_t crc_HH;
  uint16_t crc_LO;

  for (int i=0; i<size; i++)
  {
    buffData[i] = data[i];
  }

  uint16_t crc = crc16(buffData, size);
  crc_LO = crc&0xFF;   // CRC LOW
  crc_HH = (crc>>8)&0xFF;  // CRC HIGH

  if(crc_HH == data[size+1] && crc_LO == data[size] )
  {
    ret = true;
  }

  return ret;
}

void modbusException (uint8_t exceptioncode)
{
  //| SLAVE_ID | FUNCTION_CODE | Exception code | CRC     |
  //| 1 BYTE   |  1 BYTE       |    1 BYTE      | 2 BYTES |

  TxData[0] = RxData[0];       // slave ID
  TxData[1] = RxData[1]|0x80;  // adding 1 to the MSB of the function code
  TxData[2] = exceptioncode;   // Load the Exception code
  sendData(TxData, 3);         // send Data... CRC will be calculated in the function
}


uint8_t readHoldingRegs (uint16_t* Holding_Registers_Database)
{

  /*check RX Data CRC*/
  if (checkData(RxData, 6) != true)  // maximum no. of Registers as per the PDF
    {
      modbusException (ILLEGAL_DATA_VALUE);  // send an exception
      return 0;
    }

  uint16_t startAddr = ((RxData[2]<<8)|RxData[3]);  // start Register Address
  uint16_t numRegs = ((RxData[4]<<8)|RxData[5]);   // number to registers master has requested
  if ((numRegs<1)||(numRegs>125))  // maximum no. of Registers as per the PDF
  {
    modbusException (ILLEGAL_DATA_VALUE);  // send an exception
    return 0;
  }

  uint16_t endAddr = startAddr+numRegs-1;  // end Register
  if (endAddr>49)  // end Register can not be more than 49 as we only have record of 50 Registers in total
  {
    modbusException(ILLEGAL_DATA_ADDRESS);   // send an exception
    return 0;
  }

  // Prepare TxData buffer

  //| SLAVE_ID | FUNCTION_CODE | BYTE COUNT | DATA      | CRC     |
  //| 1 BYTE   |  1 BYTE       |  1 BYTE    | N*2 BYTES | 2 BYTES |

  TxData[0] = SLAVE_ID;  // slave ID
  TxData[1] = RxData[1];  // function code
  TxData[2] = numRegs*2;  // Byte count
  int indx = 3;  // we need to keep track of how many bytes has been stored in TxData Buffer

  for (int i=0; i<numRegs; i++)   // Load the actual data into TxData buffer
  {
    TxData[indx++] = (Holding_Registers_Database[startAddr]>>8)&0xFF;  // extract the higher byte
    TxData[indx++] = (Holding_Registers_Database[startAddr])&0xFF;   // extract the lower byte
    startAddr++;  // increment the register address
  }

  sendData(TxData, indx);  // send data... CRC will be calculated in the function itself
  return 1;   // success
}

uint8_t readInputRegs (uint16_t* Input_Registers_Database)
{
  /*check RX Data CRC*/
  if (checkData(RxData, 6) != true)  // maximum no. of Registers as per the PDF
    {
      modbusException (ILLEGAL_DATA_VALUE);  // send an exception
      return 0;
    }

  uint16_t startAddr = ((RxData[2]<<8)|RxData[3]);  // start Register Address
  uint16_t numRegs = ((RxData[4]<<8)|RxData[5]);   // number to registers master has requested
  if ((numRegs<1)||(numRegs>125))  // maximum no. of Registers as per the PDF
  {
    modbusException (ILLEGAL_DATA_VALUE);  // send an exception
    return 0;
  }

  uint16_t endAddr = startAddr+numRegs-1;  // end Register
  if (endAddr>49)  // end Register can not be more than 49 as we only have record of 50 Registers in total
  {
    modbusException(ILLEGAL_DATA_ADDRESS);   // send an exception
    return 0;
  }

  // Prepare TxData buffer

  //| SLAVE_ID | FUNCTION_CODE | BYTE COUNT | DATA      | CRC     |
  //| 1 BYTE   |  1 BYTE       |  1 BYTE    | N*2 BYTES | 2 BYTES |

  TxData[0] = SLAVE_ID;  // slave ID
  TxData[1] = RxData[1];  // function code
  TxData[2] = numRegs*2;  // Byte count
  int indx = 3;  // we need to keep track of how many bytes has been stored in TxData Buffer

  for (int i=0; i<numRegs; i++)   // Load the actual data into TxData buffer
  {
    TxData[indx++] = (Input_Registers_Database[startAddr]>>8)&0xFF;  // extract the higher byte
    TxData[indx++] = (Input_Registers_Database[startAddr])&0xFF;   // extract the lower byte
    startAddr++;  // increment the register address
  }
  //uartPrintf(_DEF_UART1, "%X", Input_Registers_Database[0]);
  sendData(TxData, indx);  // send data... CRC will be calculated in the function itself
  return 1;   // success
}

uint8_t writeSingleReg (uint16_t* Holding_Registers_Database)
{

  /*check RX Data CRC*/
  if (checkData(RxData, 6)!=true)  // maximum no. of Registers as per the PDF
    {
      modbusException (ILLEGAL_DATA_VALUE);  // send an exception
      return 0;
    }

  uint16_t startAddr = ((RxData[2]<<8)|RxData[3]);  // start Register Address
  if (startAddr>49)  // The Register Address can not be more than 49 as we only have record of 50 Registers in total
  {
    modbusException(ILLEGAL_DATA_ADDRESS);   // send an exception
    return 0;
  }

  /* Save the 16 bit data
   * Data is the combination of 2 bytes, RxData[4] and RxData[5]
   */

  Holding_Registers_Database[startAddr] = (RxData[4]<<8)|RxData[5];

  // Prepare Response

  //| SLAVE_ID | FUNCTION_CODE | Start Addr | Data     | CRC     |
  //| 1 BYTE   |  1 BYTE       |  2 BYTE    | 2 BYTES  | 2 BYTES |

  TxData[0] = SLAVE_ID;    // slave ID
  TxData[1] = RxData[1];   // function code
  TxData[2] = RxData[2];   // Start Addr HIGH Byte
  TxData[3] = RxData[3];   // Start Addr LOW Byte
  TxData[4] = RxData[4];   // Reg Data HIGH Byte
  TxData[5] = RxData[5];   // Reg Data LOW  Byte

  sendData(TxData, 6);  // send data... CRC will be calculated in the function itself

  /* Change CLI Mode State */
  if(startAddr == 7 &&((RxData[4]<<8)|RxData[5]) == 0xffff)
  {
    return 2;
  }

  return 1;   // success
}

