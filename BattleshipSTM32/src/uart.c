#include "uart.h"
#include "state_machine.h"

void UART_Init(void) {
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    GPIOA->MODER |= (0x2 << GPIO_MODER_MODER2_Pos) | (0x2 << GPIO_MODER_MODER3_Pos);
    GPIOA->AFR[0] |= (0x1 << GPIO_AFRL_AFRL2_Pos) | (0x1 << GPIO_AFRL_AFRL3_Pos);
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
    USART2->BRR = (SystemCoreClock / 9600);
    USART2->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_UE | USART_CR1_RXNEIE;
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

void USART2_IRQHandler(void) {
    if (USART2->ISR & USART_ISR_RXNE) {
        static char rx_buffer[100];
        static int rx_index = 0;

        char c = (char)(USART2->RDR);
        if (c == '\n' || rx_index >= sizeof(rx_buffer) - 1) {
            rx_buffer[rx_index] = '\0';
            handle_received_message(rx_buffer);
            rx_index = 0;
        } else {
            rx_buffer[rx_index++] = c;
        }

        UART_SendChar(c);
    }
}
