#include "uart.h"

void UART_Init(void) {
    // Enable the peripheral clock of GPIOA
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

    // PA2 and PA3 for USART2
    GPIOA->MODER |= (0x2 << GPIO_MODER_MODER2_Pos) | (0x2 << GPIO_MODER_MODER3_Pos);  // Alternate function mode
    GPIOA->AFR[0] |= (0x1 << GPIO_AFRL_AFRL2_Pos) | (0x1 << GPIO_AFRL_AFRL3_Pos);    // AF1 for USART2

    // Enable the peripheral clock of USART2
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

    // Configure USART2: 
    // - Baud rate: 9600
    // - 8 data bits, 1 stop bit, no parity, no flow control
    USART2->BRR = (SystemCoreClock / 9600);   // Assuming 48MHz PCLK
    USART2->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;

    // Enable USART2 interrupt
    NVIC_SetPriority(USART2_IRQn, 1);
    NVIC_EnableIRQ(USART2_IRQn);
}

void UART_SendChar(char c) {
    while (!(USART2->ISR & USART_ISR_TXE));
    USART2->TDR = c;
}

void UART_SendString(const char *str) {
    while (*str) {
        UART_SendChar(*str++);
    }
}

char UART_GetChar(void) {
    while (!(USART2->ISR & USART_ISR_RXNE));
    return (char)(USART2->RDR);
}

// USART2 interrupt handler
void USART2_IRQHandler(void) {
    if (USART2->ISR & USART_ISR_RXNE) {
        char c = (char)(USART2->RDR); // Read received data
        UART_SendChar(c); // Echo received character
    }
}
