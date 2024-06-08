// state_machine.c
#include "state_machine.h"

typedef enum {
    INIT,
    START_S1,
    START_S2,
    FIELD,
    PLAY,
    RESULT,
    GAMEEND,
    ERROR
} State;

static State currentState;

void StateMachine_Init(void) {
    currentState = INIT;
}

void StateMachine_Run(void) {
    switch (currentState) {
        case INIT:
            // Initialization code
            currentState = START_S1;
            break;
        case START_S1:
            // Code for START_S1 state
            break;
        case START_S2:
            // Code for START_S2 state
            break;
        case FIELD:
            // Code for FIELD state
            break;
        case PLAY:
            // Code for PLAY state
            break;
        case RESULT:
            // Code for RESULT state
            break;
        case GAMEEND:
            // Code for GAMEEND state
            break;
        case ERROR:
            // Code for ERROR state
            break;
    }
}
