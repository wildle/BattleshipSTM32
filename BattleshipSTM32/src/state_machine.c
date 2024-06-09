#include "state_machine.h"
#include "uart.h"

// Initial State
static State currentState = INIT;

void StateMachine_Init(void) {
    // Initial setup if necessary
    currentState = INIT;
}

void StateMachine_Run(void) {
    switch (currentState) {
        case INIT:
            // Initialization logic
            UART_SendString("State: INIT\n");
            currentState = START_S1; // Example transition
            break;

        case START_S1:
            // Logic for START_S1 state
            UART_SendString("State: START_S1\n");
            currentState = PLAY; // Example transition
            break;

        case START_S2:
            // Logic for START_S2 state
            UART_SendString("State: START_S2\n");
            currentState = PLAY; // Example transition
            break;

        case FIELD:
            // Logic for FIELD state
            UART_SendString("State: FIELD\n");
            break;

        case PLAY:
            // Logic for PLAY state
            UART_SendString("State: PLAY\n");
            currentState = RESULT; // Example transition
            break;

        case RESULT:
            // Logic for RESULT state
            UART_SendString("State: RESULT\n");
            currentState = GAMEEND; // Example transition
            break;

        case GAMEEND:
            // Logic for GAMEEND state
            UART_SendString("State: GAMEEND\n");
            currentState = INIT; // Example transition
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
