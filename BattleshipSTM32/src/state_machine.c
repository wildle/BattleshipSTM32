#include "state_machine.h"
#include "uart.h"
#include <string.h>
#include <stdio.h>

// Initial State
static State currentState = INIT;
static int startButtonPressed = 0;

void handle_button_press(void) {
    startButtonPressed = 1;
}

void send_start_message(void) {
    UART_SendString("START12345\n"); // Beispiel Matrikelnummer
}

void send_cs_message(const char *cs) {
    UART_SendString("CS");
    UART_SendString(cs);
    UART_SendChar('\n');
}

void send_boom_message(int x, int y) {
    char buffer[10];
    sprintf(buffer, "BOOM%d%d\n", x, y);
    UART_SendString(buffer);
}

void handle_received_message(const char *message) {
    if (strncmp(message, "START", 5) == 0) {
        UART_SendString("Received START message\n");
    } else if (strncmp(message, "CS", 2) == 0) {
        UART_SendString("Received CS message\n");
    } else if (strncmp(message, "BOOM", 4) == 0) {
        UART_SendString("Received BOOM message\n");
    }
}

void StateMachine_Init(void) {
    currentState = INIT;
}

void StateMachine_Run(void) {
    switch (currentState) {
        case INIT:
            UART_SendString("State: INIT\n");
            if (startButtonPressed) {
                send_start_message();
                currentState = START_S1;
                startButtonPressed = 0; // Reset button state
            }
            break;

        case START_S1:
            UART_SendString("State: START_S1\n");
            // Wait for START message
            break;

        case START_S2:
            UART_SendString("State: START_S2\n");
            // Wait for CS message
            break;

        case FIELD:
            UART_SendString("State: FIELD\n");
            // Wait for field setup
            break;

        case PLAY:
            UART_SendString("State: PLAY\n");
            // Handle game logic
            break;

        case RESULT:
            UART_SendString("State: RESULT\n");
            currentState = GAMEEND;
            break;

        case GAMEEND:
            UART_SendString("State: GAMEEND\n");
            if (startButtonPressed) {
                currentState = INIT;
                startButtonPressed = 0; // Reset button state
            }
            break;

        case ERROR_STATE:
            UART_SendString("State: ERROR\n");
            break;

        case UNEXPECTED:
            UART_SendString("State: UNEXPECTED\n");
            break;

        case MYBAD:
            UART_SendString("State: MYBAD\n");
            break;

        default:
            UART_SendString("State: UNKNOWN\n");
            break;
    }
}
