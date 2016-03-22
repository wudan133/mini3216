#include "typedef.h"
#include "UART.h"
#include "DS1302.H"

#define FSA_SYNC    0
#define FSA_CMD     1
#define FSA_LEN     2
#define FSA_DATA    3
#define FSA_CRC     4

#define TLNK_SYNC               0x10
#define TLNKCMD_ACK             0x06
#define TLNKCMD_NACK            0x21
#define TLNKCMD_DEBUG           0x30
#define TLNKCMD_GPIORE          0x62
#define TLNKCMD_GPIOWR          0x63
#define TLNKCMD_GENI2CR         0x66
#define TLNKCMD_GENI2CW         0x67
#define TLNKCMD_GENI2CR16       0x68
#define TLNKCMD_GENI2CW16       0x69
#define TLNKCMD_WRITEREG        0x70
#define TLNKCMD_READREG         0x71
#define TLNKCMD_INVALID         0xff

#define BUFF_NUM 10

static UINT8 Register[10] = {0};
static CHAR  buffer_in[BUFF_NUM];
static CHAR  buffer_out[BUFF_NUM];
static UINT8 buf_count = 0;
static UINT8 state = FSA_SYNC;
static UINT8 process = 0;
static UINT8 crc = 0;
static UINT8 len = 0;
static UINT8 cnt = 0;

static BOOL g_e_is_debug = FALSE;

/*----------------------------
Init UART
Input: u16_baud (baudrate)
Output:None
----------------------------*/
void UartInit(UINT16 u16_baud)
{
    SCON = 0x50;   //8 bit data length, allow receive control bit. 
    PCON &= 0x7f;  //no frequency double
    AUXR |= 0x15;  //1T (FOSC), choose brt baudrate generator as the uart baudrate generator
    BRT = 256 - FOSC / u16_baud / 32;  //caculate the BRT register value, based on the frequency double = 0 and division is 1 
    ES = 1;
    
    EA = 1;                 //Open master interrupt switch
}

void Sendbyte(UINT8 u8_SendBuff)
{
    SBUF = u8_SendBuff;
    while(TI == 0);
    TI = 0;
}

void SendString(UINT8 *p_string)
{
    while(*p_string != '\0')
    {
        Sendbyte(*p_string);
        p_string++;
    }
}

void _send_string_com(UINT8 *str, UINT8 u8_len)
{
    UINT8 i = 0;  
    
    do  
    {  
        Sendbyte(*(str + i));  
        i ++;  
    } while (i < u8_len);
}

void _clean(void)
{
    buf_count = 0;
    state = FSA_SYNC;
}

void _answer_noack(void)
{
    /* send NACK sothat sender will re-send it */
    buffer_out[0]   = TLNK_SYNC;
    buffer_out[1]   = TLNKCMD_NACK;
    buffer_out[2]   = 0;
    buffer_out[3]   = TLNK_SYNC + TLNKCMD_NACK + 0;
    _send_string_com(buffer_out, 4);
}


/*----------------------------
UART interrupt service routine
----------------------------*/
void Uart_Isr() interrupt 4
{
    if (RI)
    {
        buffer_in[buf_count] = SBUF;

        switch (state)
        {
        case FSA_SYNC:
            if (buffer_in[buf_count] != TLNK_SYNC)
            {
                _clean();
                break;
            }
            crc = buffer_in[buf_count];
            buf_count = 1;
            state = FSA_CMD;
            break;
        case FSA_CMD:
            crc += buffer_in[buf_count];
            buf_count = 2;
            state = FSA_LEN;
            break;
        case FSA_LEN:
            len = buffer_in[buf_count];
            if (len == 0)
            {
                buf_count = 3;
                state = FSA_CRC;
            }
            else
            {
                cnt = 0;
                crc += buffer_in[buf_count];
                buf_count = 3;
                state = FSA_DATA;
            }
            break;
        case FSA_DATA:
            crc += buffer_in[buf_count];
            buf_count ++;
            cnt ++;
            if (cnt == len)
            {
                state = FSA_CRC;
            }
            break;
         case FSA_CRC:
            if (crc != buffer_in[buf_count]) 
            {
                _clean();
                break;
            }
            memcpy(buffer_out, buffer_in, BUFF_NUM);
            process = 1;
            _clean();
            break;
        default:
            _clean();
            break;
        }
        RI = 0;
    }
}

void _debug_service(void)
{    
    UINT8 cmd = TLNKCMD_INVALID;
#if 0
    UINT16 subaddr;
#endif

    while (process)
    {        
        cmd = buffer_out[1];

        buffer_out[0]   = TLNK_SYNC;
        buffer_out[1]   = TLNKCMD_ACK;
        crc = buffer_out[0] + buffer_out[1];
        
        switch (cmd)
        {
    #if 0
        case TLNKCMD_GENI2CR:
            buffer_out[2] = 2;
            buffer_out[3] = 0;
            buffer_out[4] = mculib_i2c_read_8bidx8bval(buffer_out[5], buffer_out[6]);
            crc += buffer_out[2] + buffer_out[3] + buffer_out[4];
            buffer_out[5] = crc;
            _send_string_com(buffer_out, 6);
            break;
        case TLNKCMD_GENI2CW:
            buffer_out[2] = 1;
            if (mculib_i2c_write_8bidx8bval(buffer_out[5], buffer_out[6], buffer_out[7]))
            {
                buffer_out[3] = 0;
            }
            else
            {
                buffer_out[3] = 1;
            }
            crc += buffer_out[2] + buffer_out[3];
            buffer_out[4] = crc;
            _send_string_com(buffer_out, 5);
            break;
        case TLNKCMD_GENI2CR16:
            buffer_out[2] = 2;
            buffer_out[3] = 0;
            subaddr = (UINT8)buffer_out[7];
            subaddr <<= 8;
            subaddr += (UINT8)buffer_out[6];
            buffer_out[4] = mculib_i2c_read_16bidx8bval(buffer_out[5], subaddr);
            crc += buffer_out[2] + buffer_out[3] + buffer_out[4];
            buffer_out[5] = crc;
            _send_string_com(buffer_out, 6);
            break;
        case TLNKCMD_GENI2CW16:
            buffer_out[2] = 1;
            subaddr = (UINT8)buffer_out[7];
            subaddr <<= 8;
            subaddr += (UINT8)buffer_out[6];
            if (mculib_i2c_write_16bidx8bval(buffer_out[5], subaddr, buffer_out[8]))
            {
                buffer_out[3] = 0;
            }
            else
            {
                buffer_out[3] = 1;
            }
            crc += buffer_out[2] + buffer_out[3];
            buffer_out[4] = crc;
            _send_string_com(buffer_out, 5);
            break;
     #endif
        case TLNKCMD_DEBUG:
            buffer_out[2] = 0;
            g_e_is_debug = buffer_out[3];        
            buffer_out[3] = crc;
            _send_string_com(buffer_out, 4);
//            if (g_e_is_debug) //sned hex: 10 30 01 01 42
//            {
//                LOG("debug.");
//            }
//            else //sned hex: 10 30 01 00 41
//            {
//                LOG("run.");
//            }
            break;
    #if 0
        case TLNKCMD_GPIORE:    //0x62
        {
            switch (buffer_out[3])
            {
                case 0: buffer_out[6] = P0; break;
                case 1: buffer_out[6] = P1; break;
                case 2: buffer_out[6] = P2; break;
                case 3: buffer_out[6] = P3; break;
                case 4: buffer_out[6] = P4; break;
                default: _answer_noack(); process = 0; return;
            }
            if (buffer_out[2] == 2)
            {
                if (buffer_out[4] >= 8)
                {
                    _answer_noack();
                    process = 0;
                    return;
                }
                buffer_out[6] = (buffer_out[6] >> buffer_out[4]) & 0x01;
            }
            if (buffer_out[2] != 1 && buffer_out[2] != 2)
            {
                _answer_noack();
                process = 0;
                return;
            }
            buffer_out[2] = 2;
            buffer_out[3] = 0;
            buffer_out[4] = buffer_out[6];
            crc += buffer_out[2] + buffer_out[3] + buffer_out[4];
            buffer_out[5] = crc;
            _send_string_com(buffer_out, 6);//10 06 02 00 val crc
            break;
        }
        case TLNKCMD_GPIOWR:    //0x63
        {
            if (buffer_out[2] == 3)
            {
                if (buffer_out[5] == 1)
                {
                    switch (buffer_out[4])
                    {
                        case 0: buffer_out[6] = 0x01; break;
                        case 1: buffer_out[6] = 0x02; break;
                        case 2: buffer_out[6] = 0x04; break;
                        case 3: buffer_out[6] = 0x08; break;
                        case 4: buffer_out[6] = 0x10; break;
                        case 5: buffer_out[6] = 0x20; break;
                        case 6: buffer_out[6] = 0x40; break;
                        case 7: buffer_out[6] = 0x80; break;
                        default: _answer_noack(); process = 0; return;
                    }
                    switch (buffer_out[3])
                    {
                        case 0: P0 |= buffer_out[6]; break;
                        case 1: P1 |= buffer_out[6]; break;
                        case 2: P2 |= buffer_out[6]; break;
                        case 3: P3 |= buffer_out[6]; break;
                        case 4: P4 |= buffer_out[6]; break;
                        default: _answer_noack(); process = 0; return;
                    }
                }
                else
                {
                    switch (buffer_out[4])
                    {
                        case 0: buffer_out[6] = 0xfe; break;
                        case 1: buffer_out[6] = 0xfd; break;
                        case 2: buffer_out[6] = 0xfb; break;
                        case 3: buffer_out[6] = 0xf7; break;
                        case 4: buffer_out[6] = 0xef; break;
                        case 5: buffer_out[6] = 0xdf; break;
                        case 6: buffer_out[6] = 0xbf; break;
                        case 7: buffer_out[6] = 0x7f; break;
                        default: _answer_noack(); process = 0; return;
                    }
                    switch (buffer_out[3])
                    {
                        case 0: P0 &= buffer_out[6]; break;
                        case 1: P1 &= buffer_out[6]; break;
                        case 2: P2 &= buffer_out[6]; break;
                        case 3: P3 &= buffer_out[6]; break;
                        case 4: P4 &= buffer_out[6]; break;
                        default: _answer_noack(); process = 0; return;
                    }
                }
            }
            else if (buffer_out[2] == 2)
            {
                switch (buffer_out[3])
                {
                    case 0: P0 = buffer_out[4]; break;
                    case 1: P1 = buffer_out[4]; break;
                    case 2: P2 = buffer_out[4]; break;
                    case 3: P3 = buffer_out[4]; break;
                    case 4: P4 = buffer_out[4]; break;
                    default: _answer_noack(); process = 0; return;
                }
            }
            else
            {
                _answer_noack();
                process = 0;
                return;
            }
            buffer_out[2] = 1;
            buffer_out[3] = 0;
            crc += buffer_out[2] + buffer_out[3];
            buffer_out[4] = crc;
            _send_string_com(buffer_out, 5);// 10 06 01 00 crc
            break;
        }
    #endif
        case TLNKCMD_WRITEREG: //0x70
            if (buffer_out[2] == 2)
            {
                if(buffer_out[3] == 0)
                {
                    g_Timerbuff[1] = Register[2];
                    g_Timerbuff[2] = Register[3];
                    g_Timerbuff[3] = Register[4];
                    g_Timerbuff[4] = Register[5];
                    g_Timerbuff[5] = Register[6];
                    g_Timerbuff[6] = Register[7];
                    g_Timerbuff[7] = Register[8];
                    
                    Ds1302_Write_Time(g_Timerbuff);
                    buffer_out[3] = 1;
                }
                else if (buffer_out[3] < 10)
                {
                    Register[buffer_out[3]]  = buffer_out[4];
                    buffer_out[3] = 2;
                }
                else
                {
                    _answer_noack();
                    process = 0;
                    return;
                }
            }
            else
            {
                _answer_noack();
                process = 0;
                return;
            }
            
            buffer_out[2] = 1;
            crc += buffer_out[2] + buffer_out[3];
            buffer_out[4] = crc;
            _send_string_com(buffer_out, 5);// 10 06 01 0X crc
            break;
        case TLNKCMD_READREG:  //0x71
            if (buffer_out[2] == 1)
            {
                buffer_out[3] = Register[buffer_out[3]];
            }
            else
            {
                _answer_noack();
                process = 0;
                return;
            }
            
            buffer_out[2] = 1;
            crc += buffer_out[2] + buffer_out[3];
            buffer_out[4] = crc;
            _send_string_com(buffer_out, 5);// 10 06 01 0X crc
            break;
            
        default:
            _answer_noack();
            break;
        }
        process = 0;
    }
}

void mculib_uart_service(void)
{   
    do
    {
        _debug_service();
    } while(g_e_is_debug);
}
