#ifndef _UART_H
#define _UART_H
      
void UART_Initialize(void);
void UART_Write(unsigned char data);
void UART_Write_Text(char* text);

#endif