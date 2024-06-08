#ifndef UART_H
#define UART_H

void UART_Init(void);
void UART_SendString(const char* str);
char UART_GetChar(void);
void UART_SendChar(char c);

#endif // UART_H
