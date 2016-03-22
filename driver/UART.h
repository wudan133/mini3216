#ifndef __UART_H__
#define __UART_H__



void Sendbyte(UINT8 u8_SendBuff);
void SendString(UINT8 *p_string);

void UartInit(UINT16 u16_baud);
void mculib_uart_service(void);

#endif
