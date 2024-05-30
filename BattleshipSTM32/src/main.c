#include "stm32f0xx.h"
#include <stdlib.h>
#include <time.h>

#define FIELD_SIZE 10
#define MATRIKELNUMMER "1234"  // Ersetze dies durch deine tatsächliche Matrikelnummer

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
    USART2->CR1 |= USART_CR1_TE | USART_CR1_RE;
    USART2->CR1 |= USART_CR1_UE;
}

void UART_SendChar(char c) {
    while (!(USART2->ISR & USART_ISR_TXE));
    USART2->TDR = c;
}

char UART_GetChar(void) {
    while (!(USART2->ISR & USART_ISR_RXNE));
    return USART2->RDR;
}

void UART_SendString(const char* str) {
    while (*str) {
        UART_SendChar(*str++);
    }
}

void GPIO_Init(void) {
    // Enable the peripheral clock of GPIOC
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;

    // Configure PC13 as input (Button B1)
    GPIOC->MODER &= ~(0b11 << GPIO_MODER_MODER13_Pos);
}

int isButtonPressed(void) {
    // Check if the button is pressed (active low)
    return !(GPIOC->IDR & GPIO_IDR_13);
}

typedef enum {
    STATE_INIT,
    STATE_WAIT_FOR_BUTTON,
    STATE_START_S1,
    STATE_START_S2,
    STATE_FIELD,
    STATE_PLAY,
    STATE_RESULT,
    STATE_GAMEEND,
    STATE_ERROR,
    STATE_UNEXPECTED,
    STATE_MYBAD
} MainStates;

MainStates currentState = STATE_INIT;

// Spielfeld und Checksummenvariablen
int field[FIELD_SIZE][FIELD_SIZE];
char checksum[11];

void generateRandomField() {
    srand(time(NULL));
    for (int i = 0; i < FIELD_SIZE; i++) {
        for (int j = 0; j < FIELD_SIZE; j++) {
            field[i][j] = rand() % 2;  // 0 für Wasser, 1 für Schiffsteil
        }
    }
}

void calculateChecksum() {
    for (int i = 0; i < FIELD_SIZE; i++) {
        int sum = 0;
        for (int j = 0; j < FIELD_SIZE; j++) {
            sum += field[j][i];
        }
        checksum[i] = sum + '0';  // ASCII Konvertierung
    }
    checksum[FIELD_SIZE] = '\0';  // Null-Terminator für String
}

void StateMachine_Update(void) {
    char receivedChar;

    switch (currentState) {
        case STATE_INIT:
            // Initial state actions
            UART_SendString("INIT\n");
            currentState = STATE_WAIT_FOR_BUTTON;  // Wait for button press
            break;

        case STATE_WAIT_FOR_BUTTON:
            // Wait for button press to start
            if (isButtonPressed()) {
                char startMessage[16];
                snprintf(startMessage, sizeof(startMessage), "START%s\n", MATRIKELNUMMER);
                UART_SendString(startMessage);
                generateRandomField();
                calculateChecksum();
                currentState = STATE_FIELD;
            }
            break;

        case STATE_START_S1:
            // Start S1 state actions
            {
                char startMessage[16];
                snprintf(startMessage, sizeof(startMessage), "START%s\n", MATRIKELNUMMER);
                UART_SendString(startMessage);
            }
            generateRandomField();
            calculateChecksum();
            currentState = STATE_FIELD;
            break;

        case STATE_START_S2:
            // Start S2 state actions
            receivedChar = UART_GetChar();
            if (receivedChar == 'S') {
                char startMessage[16];
                snprintf(startMessage, sizeof(startMessage), "START%s\n", MATRIKELNUMMER);
                UART_SendString(startMessage);
                generateRandomField();
                calculateChecksum();
                currentState = STATE_FIELD;
            }
            break;

        case STATE_FIELD:
            // Field state actions
            UART_SendString("CS");
            UART_SendString(checksum);
            UART_SendString("\n");
            currentState = STATE_PLAY;
            break;

        case STATE_PLAY:
            // Play state actions
            UART_SendString("BOOM00\n");  // Example shot, update with actual game logic
            receivedChar = UART_GetChar();
            if (receivedChar == 'T' || receivedChar == 'W') {
                // Process hit or miss
                currentState = STATE_RESULT;  // This is just an example transition
            }
            break;

        case STATE_RESULT:
            // Result state actions
            UART_SendString("RESULT\n");  // Example result action
            currentState = STATE_GAMEEND;
            break;

        case STATE_GAMEEND:
            // Game end actions
            UART_SendString("GAMEEND\n");
            currentState = STATE_INIT;  // Ready for a new game
            break;

        case STATE_ERROR:
            // Error state actions
            UART_SendString("ERROR\n");
            currentState = STATE_INIT;  // Reset state machine
            break;

        case STATE_UNEXPECTED:
            // Unexpected state actions
            UART_SendString("UNEXPECTED\n");
            currentState = STATE_INIT;  // Reset state machine
            break;

        case STATE_MYBAD:
            // My bad state actions
            UART_SendString("MYBAD\n");
            currentState = STATE_INIT;  // Reset state machine
            break;

        default:
            currentState = STATE_ERROR;  // Should never reach here
            break;
    }
}

int main(void) {
    // System Initialization
    SystemInit();
    UART_Init();
    GPIO_Init();

    while (1) {
        StateMachine_Update();
    }
}
