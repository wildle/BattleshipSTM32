#include "state_machine.h"
#include "uart.h"
#include <string.h>
#include <stdio.h>

// Initial State
static State currentState = INIT;
static int start_condition_met = 0;
static int play_condition_met = 0;
static int message_received = 0;
static char received_message[100];

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
    strncpy(received_message, message, sizeof(received_message) - 1);
    received_message[sizeof(received_message) - 1] = '\0';
    message_received = 1;
}

void process_message(void) {
    if (strncmp(received_message, "START", 5) == 0) {
        // Handle START message
        UART_SendString("Processed START message\n");
        start_condition_met = 1;
    } else if (strncmp(received_message, "CS", 2) == 0) {
        // Handle CS message
        UART_SendString("Processed CS message\n");
        play_condition_met = 1;
    } else if (strncmp(received_message, "BOOM", 4) == 0) {
        // Handle BOOM message
        UART_SendString("Processed BOOM message\n");
    }
    message_received = 0;
}

void StateMachine_Init(void) {
    // Initial setup if necessary
    currentState = INIT;
    start_condition_met = 0;
    play_condition_met = 0;
    message_received = 0;
}

void StateMachine_Run(void) {
    if (message_received) {
        process_message();
    }

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
            if (start_condition_met) {
                currentState = FIELD; // Set to FIELD to process next stage before PLAY
                start_condition_met = 0; // Reset condition
                UART_SendString("Transition to FIELD\n");
            }
            break;

        case START_S2:
            // Logic for START_S2 state
            UART_SendString("State: START_S2\n");
            if (start_condition_met) {
                currentState = PLAY;
                start_condition_met = 0; // Reset condition
            }
            break;

        case FIELD:
            // Logic for FIELD state
            UART_SendString("State: FIELD\n");
            if (play_condition_met) {
                currentState = PLAY;
                play_condition_met = 0; // Reset condition
                UART_SendString("Transition to PLAY\n");
            }
            break;

        case PLAY:
            // Logic for PLAY state
            UART_SendString("State: PLAY\n");
            if (play_condition_met) {
                currentState = RESULT;
                play_condition_met = 0; // Reset condition
                UART_SendString("Transition to RESULT\n");
            }
            break;

        case RESULT:
            // Logic for RESULT state
            UART_SendString("State: RESULT\n");
            currentState = GAMEEND;
            break;

        case GAMEEND:
            // Logic for GAMEEND state
            UART_SendString("State: GAMEEND\n");
            currentState = INIT;
            break;

        case ERROR_STATE:
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
