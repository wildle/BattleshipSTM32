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
            // Wait for some condition or event
            // Example: If a start button is pressed or a specific UART message is received
            // For now, transition to next state for testing purposes
            currentState = PLAY;
            break;

        case START_S2:
            // Logic for START_S2 state
            UART_SendString("State: START_S2\n");
            // Wait for some condition or event
            // Example: If a specific UART message is received
            // For now, transition to next state for testing purposes
            currentState = PLAY;
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
            // For now, transition back to INIT for testing purposes
            currentState = INIT;
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
