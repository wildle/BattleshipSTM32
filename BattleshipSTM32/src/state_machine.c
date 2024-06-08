#include "state_machine.h"
#include "uart.h"
#include <stdio.h>
#include "stm32f0xx.h"

typedef enum {
    INIT,
    START_S1,
    START_S2,
    FIELD,
    PLAY,
    RESULT,
    GAMEEND,
    STATE_ERROR
} State;

static State currentState;
static int isPlayer1 = 0;

void StateMachine_Init(void) {
    // Check if button is pressed
    if ((GPIOC->IDR & GPIO_IDR_13) == 0) {
        isPlayer1 = 1;
    } else {
        isPlayer1 = 0;
    }
    currentState = INIT;
}

void StateMachine_Run(void) {
    switch (currentState) {
        case INIT:
            // Initialization code
            UART_SendString("Initializing...\n");
            currentState = (isPlayer1) ? START_S1 : START_S2;
            break;
        case START_S1:
            // Code for START_S1 state
            UART_SendString("START12345\n"); // Example start message
            currentState = PLAY;
            break;
        case START_S2:
            // Code for START_S2 state
            UART_SendString("Waiting for START...\n");
            char receivedChar = UART_ReceiveChar();
            if (receivedChar == 'S') {
                UART_SendString("Received START\n");
                currentState = PLAY;
            }
            break;
        case FIELD:
            // Code for FIELD state
            break;
        case PLAY:
            // Code for PLAY state
            UART_SendString("Playing...\n");
            // Implementiere hier die Logik für das Spiel
            break;
        case RESULT:
            // Code for RESULT state
            break;
        case GAMEEND:
            // Code for GAMEEND state
            break;
        case STATE_ERROR:
            // Code for ERROR state
            UART_SendString("Error occurred!\n");
            break;
    }
}
