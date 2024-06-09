#include "state_machine.h"
#include "uart.h"

static State currentState = INIT;

void StateMachine_Init(void) {
    // Initial setup if necessary
    currentState = INIT;
}

void StateMachine_SetState(State newState) {
    currentState = newState;
    // Sende Debug-Nachricht bei Zustandswechsel
    switch (currentState) {
        case INIT:
            UART_SendString("State: INIT\n");
            break;
        case START_S1:
            UART_SendString("State: START_S1\n");
            break;
        // Weitere Zustände entsprechend implementieren
        default:
            UART_SendString("State: UNKNOWN\n");
            break;
    }
}

void StateMachine_Run(void) {
    switch (currentState) {
        case INIT:
            // Warte auf den Startbefehl
            break;

        case START_S1:
            // Spiel-Logik für START_S1
            // Nach Abschluss wieder zu INIT wechseln
            StateMachine_SetState(INIT);
            break;

        // Weitere Zustände entsprechend implementieren

        default:
            break;
    }
}
