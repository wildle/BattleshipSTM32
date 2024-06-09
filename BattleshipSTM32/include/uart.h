#ifndef UART_H
#define UART_H

#include "stm32f0xx.h"

void UART_Init(void);
void UART_SendChar(char c);
void UART_SendString(const char *str);
char UART_GetChar(void);

#endif // UART_H
