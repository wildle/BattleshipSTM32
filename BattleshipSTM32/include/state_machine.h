#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include "stm32f0xx.h"

typedef enum {
    INIT,
    START_S1,
    START_S2,
    FIELD,
    PLAY,
    RESULT,
    GAMEEND,
    ERROR_STATE, // Rename this to avoid conflict
    UNEXPECTED,
    MYBAD
} State;

void StateMachine_Init(void);
void StateMachine_Run(void);

#endif // STATE_MACHINE_H
