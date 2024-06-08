#include "stm32f0xx.h"
#include "uart.h"

static volatile char rxBuffer[64];
static volatile uint8_t rxHead = 0;
static volatile uint8_t rxTail = 0;

void UART_Init(void) {
    // Enable the peripheral clock of GPIOA
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

    // Configure PA2 as TX, PA3 as RX
    GPIOA->MODER |= (0b10 << GPIO_MODER_MODER2_Pos) | (0b10 << GPIO_MODER_MODER3_Pos);
    GPIOA->AFR[0] |= (0b0001 << GPIO_AFRL_AFRL2_Pos) | (0b0001 << GPIO_AFRL_AFRL3_Pos);

    // Enable the peripheral clock for USART2
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

    // Configure USART2: 9600 baud, 8 data bits, no parity, 1 stop bit
    USART2->BRR = SystemCoreClock / 9600;
    USART2->CR1 |= USART_CR1_TE | USART_CR1_RE | USART_CR1_RXNEIE; // Enable transmitter, receiver, and RXNE interrupt
    USART2->CR1 |= USART_CR1_UE;

    // Enable USART2 interrupt in NVIC
    NVIC_EnableIRQ(USART2_IRQn);
}

void UART_SendChar(char c) {
    while (!(USART2->ISR & USART_ISR_TXE));
    USART2->TDR = c;
}

char UART_GetChar(void) {
    while (rxHead == rxTail); // Wait until data is available
    char c = rxBuffer[rxTail];
    rxTail = (rxTail + 1) % sizeof(rxBuffer);
    return c;
}

void UART_SendString(const char* str) {
    while (*str) {
        UART_SendChar(*str++);
    }
}

void USART2_IRQHandler(void) {
    if (USART2->ISR & USART_ISR_RXNE) {
        // Read received character and store in buffer
        rxBuffer[rxHead] = USART2->RDR;
        rxHead = (rxHead + 1) % sizeof(rxBuffer);
    }
}
