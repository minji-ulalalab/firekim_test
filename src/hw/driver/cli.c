/*
 * cli.c
 *
 *  Created on: 2023. 7. 10.
 *      Author: KIMMINJI
 */

#include "cli.h"
#include "uart.h"


#define CLI_KEY_BACK              0x08
#define CLI_KEY_DEL               0x7F
#define CLI_KEY_ENTER             0x0D
#define CLI_KEY_ESC               0x1B

#define CLI_ARGS_MAX              32
#define CLI_PRINT_BUF_MAX         256

bool cliExit_Flag = true;
bool cliStart_Flag = true;
bool cliuser_Flag = true;
bool clisaved_Flag = false;

extern uint8_t RxData[256];

typedef enum {
    INIT_STATE,
    GET_INFO_STATE,
    SET_INFO_STATE,
    SLAVE_ID_STATE,
    BUADRATE_STATE,
    //DATA_BIT_STATE,
    //STOP_BIT_STATE,
    //PARITY_BIT_STATE,
    //FLOW_CTL_STATE,
    SAVE_STATE,
    EXIT_STATE,
} cli_State_t;

cli_State_t cliState;

typedef struct
{
  uint16_t    slave_id;
  uint16_t    baud_HH;
  uint16_t    baud_LL;
  uint16_t   data_bit;
  uint16_t   stop_bit;
  uint16_t parity_bit;
  uint16_t   flow_ctl;
} cli_info_t;


typedef struct
{
  uint8_t buf[CLI_LINE_BUF_MAX];
  uint8_t buf_len;
  uint8_t cursor;
  uint8_t count;
} cli_line_t;


typedef struct
{
  uint8_t  ch;
  bool     is_open;

  char     print_buffer[CLI_PRINT_BUF_MAX];
  uint16_t  argc;
  char     *argv[CLI_ARGS_MAX];

  cli_line_t  line;

  uint16_t    cmd_count;
} cli_t;

cli_t      cli_node;
cli_info_t cli_info;

static bool cliUpdate(cli_t *p_cli, uint8_t rx_data);
static void cliLineClean(cli_t *p_cli);

void cliStateHandler(cli_t *p_cli);
void cliGettingInfo(void);
void cliExit(void);
void cliPagePrint(void);




bool cliInit(void)
{
  cli_node.is_open = false;


  cliLineClean(&cli_node);

  return true;
}

bool cliOpen(uint8_t ch)
{
  cli_node.ch = ch;

  cli_node.is_open = uartOpen(ch);

  cliExit_Flag = true;
  cliState = INIT_STATE;

  return cli_node.is_open;
}

void cliShowPrompt(cli_t *p_cli)
{
  uartPrintf(p_cli->ch, "\n\r");
  uartPrintf(p_cli->ch, "#USER >");
}

bool cliMain(uint16_t* Holding_Registers_Database)
{
  uint32_t ret = 0;

  cli_info.slave_id   = Holding_Registers_Database[0];
  cli_info.baud_HH    = Holding_Registers_Database[1];
  cli_info.baud_LL    = Holding_Registers_Database[2];
  cli_info.data_bit   = Holding_Registers_Database[3];
  cli_info.stop_bit   = Holding_Registers_Database[4];
  cli_info.parity_bit = Holding_Registers_Database[5];
  cli_info.flow_ctl   = Holding_Registers_Database[6];


  if (cli_node.is_open != true)
  {
    return false;
  }

  cliStart_Flag = true;
  cliuser_Flag = true;
  while(cliExit_Flag)
  {
    ret = uartAvailable(cli_node.ch);
    if (ret > 0)
    {
      if(cliuser_Flag)
      {
        uartRead(cli_node.ch);
        for (int i=0; i<ret; i++)
        {
          cliUpdate(&cli_node, RxData[i]);
        }
      }
    }
  }

  Holding_Registers_Database[0] = cli_info.slave_id;
  Holding_Registers_Database[1] = cli_info.baud_HH;
  Holding_Registers_Database[2] = cli_info.baud_LL;
  Holding_Registers_Database[3] = cli_info.data_bit;
  Holding_Registers_Database[4] = cli_info.stop_bit;
  Holding_Registers_Database[5] = cli_info.parity_bit;
  Holding_Registers_Database[6] = cli_info.flow_ctl;

  return true;
}

bool cliUpdate(cli_t *p_cli, uint8_t rx_data)
{
  bool ret = false;
  cli_line_t *line;

  line = &p_cli->line;

  switch(rx_data)
  {
    // 엔터
    //
    case CLI_KEY_ENTER:
      cliStateHandler(&cli_node);
      line->count = 0;
      line->cursor = 0;
      line->buf[0] = 0;
      break;

      // DEL
      //
    case CLI_KEY_DEL:
      if (line->cursor < line->count)
      {
        uint8_t mov_len;

        mov_len = line->count - line->cursor;
        for (int i=1; i<mov_len; i++)
        {
          line->buf[line->cursor + i - 1] = line->buf[line->cursor + i];
        }

        line->count--;
        line->buf[line->count] = 0;

        uartPrintf(p_cli->ch, "\x1B[1P");
      }
      break;

      // 백스페이스
      //
    case CLI_KEY_BACK:
      if (line->count > 0 && line->cursor > 0)
      {
        if (line->cursor == line->count)
        {
          line->count--;
          line->buf[line->count] = 0;
        }

        if (line->cursor < line->count)
        {
          uint8_t mov_len;

          mov_len = line->count - line->cursor;

          for (int i=0; i<mov_len; i++)
          {
            line->buf[line->cursor + i - 1] = line->buf[line->cursor + i];
          }

          line->count--;
          line->buf[line->count] = 0;
        }
      }

      if (line->cursor > 0)
      {
        line->cursor--;
        uartPrintf(p_cli->ch, "\b \b\x1B[1P");
      }
      break;


    default:
      if ((line->count + 1) < line->buf_len)
      {
        if (line->cursor == line->count)
        {
          uartWrite(p_cli->ch, &rx_data, 1);

          line->buf[line->cursor] = rx_data;
          line->count++;
          line->cursor++;
          line->buf[line->count] = 0;
        }
        if (line->cursor < line->count)
        {
          uint8_t mov_len;

          mov_len = line->count - line->cursor;
          for (int i=0; i<mov_len; i++)
          {
            line->buf[line->count - i] = line->buf[line->count - i - 1];
          }
          line->buf[line->cursor] = rx_data;
          line->count++;
          line->cursor++;
          line->buf[line->count] = 0;

          uartPrintf(p_cli->ch, "\x1B[4h%c\x1B[4l", rx_data);
        }
      }
      break;
  }
  return ret;
}

void cliLineClean(cli_t *p_cli)
{
  p_cli->line.count   = 0;
  p_cli->line.cursor  = 0;
  p_cli->line.buf_len = CLI_LINE_BUF_MAX - 1;
  p_cli->line.buf[0]  = 0;
}



void cliPrintf(const char *fmt, ...)
{
  va_list arg;
  va_start (arg, fmt);
  int32_t len;
  cli_t *p_cli = &cli_node;


  len = vsnprintf(p_cli->print_buffer, 256, fmt, arg);
  va_end (arg);
  uartWrite(p_cli->ch, (uint8_t *)p_cli->print_buffer, len);
}

void cliStateHandler(cli_t *p_cli)
{
  cli_line_t *line;

  line = &p_cli->line;
  cliuser_Flag = false;
  switch(cliState)
  {
    case INIT_STATE:
      if(cliStart_Flag)
      {
        cliStart_Flag = false;
        break;
      }
      if (line->count > 0)
      {
        if(line->buf[0] == '1')
        {
          cliState = GET_INFO_STATE;
        }
        else if(line->buf[0] == '2')
        {
          cliState = SET_INFO_STATE;
        }
        else if(line->buf[0] == '3')
        {
          cliState = EXIT_STATE;
        }
        else
        {
          cliPrintf("\r\n");
          cliPrintf("******* You can only choose from the options.*******\r\n");
          cliPrintf("**To view the options, simply press the Enter key.**\r\n");
          cliState = INIT_STATE;
        }
      }
      else
      {
        cliState = INIT_STATE;
      }

      break;

    case GET_INFO_STATE:
      if (line->count > 0)
      {
        if(line->buf[0] == '1')
        {
          cliState = INIT_STATE;
          break;
        }
        else if(line->buf[0] == '2')
        {
          cliState = SET_INFO_STATE;
        }
        else if(line->buf[0] == '3')
        {
          cliState = EXIT_STATE;
        }
        else
        {
          cliPrintf("\r\n");
          cliPrintf("******* You can only choose from the options.*******\r\n");
          cliPrintf("**To view the options, simply press the Enter key.**\r\n");
          cliState = GET_INFO_STATE;
        }
      }
      else
      {
        cliState = GET_INFO_STATE;
      }
      break;

    case SET_INFO_STATE:
      if (line->count > 0)
      {
        if(line->buf[0] == '1')
        {
          cliState = SLAVE_ID_STATE;
          clisaved_Flag = true;
        }
        else if(line->buf[0] == '2')
        {
          cliState = BUADRATE_STATE;
          clisaved_Flag = true;
        }
        /*
        else if(line->buf[0] == '3')
        {
          cliState = DATA_BIT_STATE;
        }
        else if(line->buf[0] == '4')
        {
          cliState = STOP_BIT_STATE;
        }
        else if(line->buf[0] == '5')
        {
          cliState = PARITY_BIT_STATE;
        }
        else if(line->buf[0] == '6')
        {
          cliState = FLOW_CTL_STATE;
        }
        */
        else if(line->buf[0] == '7')
        {
          if(clisaved_Flag)
          {
            cliState = SAVE_STATE;
          }
          else
          {
            cliState = INIT_STATE;
          }
        }
        else
        {
          cliPrintf("\r\n");
          cliPrintf("******* You can only choose from the options.*******\r\n");
          cliPrintf("**To view the options, simply press the Enter key.**\r\n");
          cliState = SET_INFO_STATE;
        }
      }
      else
      {
        cliState = SET_INFO_STATE;
      }

      break;

    case SLAVE_ID_STATE:
      if (line->count > 0)
      {
        if(line->buf[0] == '1')
        {
          //01
          cli_info.slave_id = 0x01;
          cliState = SET_INFO_STATE;
        }
        else if(line->buf[0] == '2')
        {
          //02
          cli_info.slave_id = 0x02;
          cliState = SET_INFO_STATE;
        }
        else if(line->buf[0] == '3')
        {
          cliState = SET_INFO_STATE;
        }
        else
        {
          cliState = SLAVE_ID_STATE;
        }
      }
      else
      {
        cliState = SLAVE_ID_STATE;
      }
      break;

    case BUADRATE_STATE:
      if (line->count > 0)
      {
        if(line->buf[0] == '1')
        {
          //9600
          cli_info.baud_HH = 0x25;
          cli_info.baud_LL = 0x80;
          cliState = SET_INFO_STATE;
        }
        else if(line->buf[0] == '2')
        {
          //19200
          cli_info.baud_HH = 0x4B;
          cli_info.baud_LL = 0x00;
          cliState = SET_INFO_STATE;
        }
        else if(line->buf[0] == '3')
        {
          //115200
          cli_info.baud_HH = 0x1C2;
          cli_info.baud_LL = 0x00;
          cliState = SET_INFO_STATE;
        }
        else if(line->buf[0] == '4')
        {
          cliState = SET_INFO_STATE;
        }
        else
        {
          cliState = SLAVE_ID_STATE;
        }
      }
      else
      {
        cliState = SLAVE_ID_STATE;
      }
      break;
    //case DATA_BIT_STATE:
    //case STOP_BIT_STATE:
    //case PARITY_BIT_STATE:
    //case FLOW_CTL_STATE:
    case SAVE_STATE:
      if (line->count > 0)
      {
        if(line->buf[0] == 'y' || line->buf[0] == 'Y')
        {
          //save to modbusmap
          cliState = INIT_STATE;
          clisaved_Flag = false;

        }
        else if(line->buf[0] == 'n' || line->buf[0] == 'N')
        {
          cliState = INIT_STATE;
          clisaved_Flag = false;
          break;
        }
        else
        {
          cliPrintf("\r\n");
          cliPrintf("******* You can only choose from the options.*******\r\n");
          cliPrintf("**To view the options, simply press the Enter key.**\r\n");
          cliState = SAVE_STATE;
        }
      }
      else
      {
        cliState = SAVE_STATE;
      }

      break;
    case EXIT_STATE:
      if (line->count > 0)
      {
        if(line->buf[0] == '1')
        {
          cliExit();
          return;
        }
        else if(line->buf[0] == '2')
        {
          cliState = INIT_STATE;
          break;
        }
        else
        {
          cliPrintf("\r\n");
          cliPrintf("******* You can only choose from the options.*******\r\n");
          cliPrintf("**To view the options, simply press the Enter key.**\r\n");
          cliState = EXIT_STATE;
        }
      }
      else
      {
        cliState = EXIT_STATE;
      }

      break;
   }

  cliPagePrint();
  cliShowPrompt(p_cli);
  cliuser_Flag = true;
}
void cliGettingInfo(void)
{
  uint32_t baudrate = (uint32_t)cli_info.baud_HH<<8 | (uint32_t)cli_info.baud_LL<<0;

  cliPrintf("\r\n");
  cliPrintf("---------- Device Info ---------\r\n");
  cliPrintf("*Slave ID: %d\r\n*Baudrate: %d\r\n*ParityBit: %d\r\n*DataBit: %d\r\n*StopBit: %d\r\n*FlowCtl: %d\r\n",
            cli_info.slave_id, baudrate, cli_info.parity_bit, cli_info.data_bit, cli_info.stop_bit, cli_info.flow_ctl);

  cliPrintf("--------------------------------\r\n");
}
void cliExit(void)
{
  cliPrintf("\r\n");
  cliPrintf("Bye! *^o^*");
  cliExit_Flag = false;
}
void cliPagePrint(void)
{
  switch(cliState)
  {
    case INIT_STATE:
      cliPrintf("\r\n***********************************************************\r\n");
      cliPrintf("**********    FIREKIM BOARD CLI PROGRAM v1.100   **********\r\n");
      cliPrintf("** ---------------------Select Option--------------------**\r\n");
      cliPrintf("** 1. GETTING BOARD INFORMATION.                         **\r\n");
      cliPrintf("** 2. SETTING BOARD INFORMATION.                         **\r\n");
      cliPrintf("** 3. EXIT                                               **\r\n");
      cliPrintf("***********************************************************\r\n");
      break;

    case GET_INFO_STATE:
      cliGettingInfo();
      cliPrintf("\r\n***********************************************************\r\n");
      cliPrintf("** Please Select Number You Want And Press Enter         **\r\n");
      cliPrintf("** ---------------------Select Option--------------------**\r\n");
      cliPrintf("** 1. SELECT MENU.                                       **\r\n");
      cliPrintf("** 2. SETTING BOARD INFORMATION.                         **\r\n");
      cliPrintf("** 3. EXIT                                               **\r\n");
      cliPrintf("***********************************************************\r\n");
      break;

    case SET_INFO_STATE:
      cliGettingInfo();
      cliPrintf("\r\n");
      cliPrintf("--------- Choose the option you want to change --------\r\n");
      //cliPrintf("1.Slave ID \r\n2.Baudrate\r\n3.ParityBit\r\n4.DataBit\r\n5.StopBit\r\n6.FlowCtl\r\n7.Back to Menu\r\n");
      cliPrintf("1.Slave ID \r\n2.Baudrate\r\n7.Back\r\n");
      cliPrintf("-------------------------------------------------------\r\n");
      break;
    case SLAVE_ID_STATE:
      cliPrintf("\r\n");
      cliPrintf("1.01\r\n2.02\r\n3.Back\r\n");
      break;
    case BUADRATE_STATE:
      cliPrintf("\r\n");
      cliPrintf("1.9600\r\n2.19200\r\n3.115200\r\n4.Back\r\n");
      break;
    case SAVE_STATE:
      cliGettingInfo();
      cliPrintf("\r\n");
      cliPrintf("Do you want to save? (y/n)\r\n");
      break;
    case EXIT_STATE:
      cliGettingInfo();
      cliPrintf("\r\n");
      cliPrintf("Do you want to close the CLI Program?\r\n");
      cliPrintf("1. Yes.\r\n");
      cliPrintf("2. No.\r\n");
      break;

  }
}



