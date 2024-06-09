#include "state_machine.h"
#include "uart.h"

static State currentState = INIT;

void StateMachine_Init(void) {
    // Initial setup if necessary
    currentState = INIT;
}

void StateMachine_SetState(State newState) {
    currentState = newState;
}

void StateMachine_Run(void) {
    switch (currentState) {
        case INIT:
            UART_SendString("State: INIT\n");
            break;

        case START_S1:
            UART_SendString("State: START_S1\n");
            // Warte auf den Startbefehl
            // Bei Empfang des Startbefehls:
            // if (start_condition) {
            //    StateMachine_SetState(PLAY);
            //}
            break;

        case START_S2:
            UART_SendString("State: START_S2\n");
            // Warte auf den Startbefehl
            // Bei Empfang des Startbefehls:
            // if (start_condition) {
            //    StateMachine_SetState(PLAY);
            //}
            break;

        case FIELD:
            UART_SendString("State: FIELD\n");
            break;

        case PLAY:
            UART_SendString("State: PLAY\n");
            // Spiel-Logik
            // if (game_over) {
            //    StateMachine_SetState(RESULT);
            //}
            break;

        case RESULT:
            UART_SendString("State: RESULT\n");
            StateMachine_SetState(GAMEEND);
            break;

        case GAMEEND:
            UART_SendString("State: GAMEEND\n");
            StateMachine_SetState(INIT); // Zurück zu INIT für eine neue Runde
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
