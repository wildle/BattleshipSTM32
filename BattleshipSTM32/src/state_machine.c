#include "state_machine.h"
#include "uart.h"
#include <string.h>
#include <stdio.h>

// Initial State
static State currentState = INIT;

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
        // Handle START message
        UART_SendString("Received START message\n");
    } else if (strncmp(message, "CS", 2) == 0) {
        // Handle CS message
        UART_SendString("Received CS message\n");
    } else if (strncmp(message, "BOOM", 4) == 0) {
        // Handle BOOM message
        UART_SendString("Received BOOM message\n");
    }
    // Add more handlers as needed
}

// Initial State
void StateMachine_Init(void) {
    // Initial setup if necessary
    currentState = INIT;
}

void StateMachine_Run(void) {
    switch (currentState) {
        case INIT:
            // Initialization logic
            UART_SendString("State: INIT\n");
            send_start_message();
            currentState = START_S1;
            break;

        case START_S1:
            // Logic for START_S1 state
            UART_SendString("State: START_S1\n");
            // Wait for some condition or event
            currentState = PLAY; // Transition for testing
            break;

        case START_S2:
            // Logic for START_S2 state
            UART_SendString("State: START_S2\n");
            // Wait for some condition or event
            currentState = PLAY; // Transition for testing
            break;

        case FIELD:
            // Logic for FIELD state
            UART_SendString("State: FIELD\n");
            // Wait for field setup to complete
            break;

        case PLAY:
            // Logic for PLAY state
            UART_SendString("State: PLAY\n");
            // For now, transition to next state for testing purposes
            currentState = RESULT;
            break;

        case RESULT:
            // Logic for RESULT state
            UART_SendString("State: RESULT\n");
            // Display result and wait for some condition or event to reset
            currentState = GAMEEND;
            break;

        case GAMEEND:
            // Logic for GAMEEND state
            UART_SendString("State: GAMEEND\n");
            currentState = INIT; // Transition for testing
            break;

        case ERROR_STATE: // Use the renamed state
            // Logic for ERROR state
            UART_SendString("State: ERROR\n");
            break;

        case UNEXPECTED:
            // Logic for UNEXPECTED state
            UART_SendString("State: UNEXPECTED\n");
            break;

        case MYBAD:
            // Logic for MYBAD state
            UART_SendString("State: MYBAD\n");
            break;

        default:
            // Default case
            UART_SendString("State: UNKNOWN\n");
            break;
    }
}
