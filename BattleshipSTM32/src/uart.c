#include "stm32f0xx.h"
#include "uart.h"

void UART_Init(void) {
    // Enable the peripheral clock of GPIO Port
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    // Enable USART peripheral clock
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
    
    // Configure GPIO pins for USART1 TX (PA9) and RX (PA10)
    GPIOA->MODER |= GPIO_MODER_MODER9_1 | GPIO_MODER_MODER10_1; // Alternate function mode
    GPIOA->AFR[1] |= (1 << 4) | (1 << 8); // AF1 for PA9 and PA10
    
    // Configure USART1
    USART1->BRR = 48000000 / 9600; // Assuming 48 MHz clock, set baud rate to 9600
    USART1->CR1 |= USART_CR1_TE | USART_CR1_RE; // Enable transmitter and receiver
    USART1->CR1 |= USART_CR1_UE; // Enable USART
}

void UART_SendString(const char* str) {
    while (*str) {
        while (!(USART1->ISR & USART_ISR_TXE)); // Wait until TX is empty
        USART1->TDR = *str++; // Transmit character
    }
}

char UART_ReceiveChar(void) {
    while (!(USART1->ISR & USART_ISR_RXNE)); // Wait until RX is not empty
    return USART1->RDR; // Return received character
}
