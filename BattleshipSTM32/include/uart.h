#ifndef UART_H
#define UART_H

void UART_Init(void);
void UART_SendString(const char* str);
char UART_ReceiveChar(void);

#endif // UART_H
