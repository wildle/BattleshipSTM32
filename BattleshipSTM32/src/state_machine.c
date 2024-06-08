#include "state_machine.h"
#include "uart.h"
#include <stdio.h>
#include "stm32f0xx.h"

#define FIELD_SIZE 10

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
static int field[FIELD_SIZE][FIELD_SIZE];
static char checksum[11];

void StateMachine_Init(void) {
    // Check if button is pressed
    if ((GPIOC->IDR & GPIO_IDR_13) == 0) {
        isPlayer1 = 1;
    } else {
        isPlayer1 = 0;
    }
    currentState = INIT;
}

void generateRandomField(void) {
    srand(time(NULL));
    for (int i = 0; i < FIELD_SIZE; i++) {
        for (int j = 0; j < FIELD_SIZE; j++) {
            field[i][j] = rand() % 2;
        }
    }
}

void calculateChecksum(void) {
    for (int i = 0; i < FIELD_SIZE; i++) {
        int sum = 0;
        for (int j = 0; j < FIELD_SIZE; j++) {
            sum += field[j][i];
        }
        checksum[i] = sum + '0';
    }
    checksum[FIELD_SIZE] = '\0';
}

void handleBoomMessage(int x, int y) {
    char response[3];
    if (x >= 0 && x < FIELD_SIZE && y >= 0 && y < FIELD_SIZE) {
        if (field[x][y] == 1) {
            field[x][y] = 0; // Schiffsteil getroffen
            snprintf(response, sizeof(response), "T\n");
        } else {
            snprintf(response, sizeof(response), "W\n");
        }
        UART_SendString(response);
    }
}

void StateMachine_Run(void) {
    char buffer[32];
    int x, y;
    static int boom_x = 0;
    static int boom_y = 0;

    switch (currentState) {
        case INIT:
            // Initialization code
            UART_SendString("Initializing...\n");
            currentState = (isPlayer1) ? START_S1 : START_S2;
            break;
        case START_S1:
            // Code for START_S1 state
            UART_SendString("START12345\n"); // Example start message
            generateRandomField();
            calculateChecksum();
            currentState = FIELD;
            break;
        case START_S2:
            // Code for START_S2 state
            UART_SendString("Waiting for START...\n");
            if (UART_GetChar() == 'S') {
                UART_SendString("Received START\n");
                generateRandomField();
                calculateChecksum();
                currentState = FIELD;
            }
            break;
        case FIELD:
            // Code for FIELD state
            snprintf(buffer, sizeof(buffer), "CS%s\n", checksum);
            UART_SendString(buffer);
            currentState = PLAY;
            break;
        case PLAY:
            // Code for PLAY state
            UART_SendString("Playing...\n");
            if (UART_GetChar() == 'B') {
                for (int i = 0; i < 4; i++) {
                    buffer[i] = UART_GetChar();
                }
                buffer[4] = '\0';
                if (sscanf(buffer, "OOM%1d%1d", &x, &y) == 2) {
                    handleBoomMessage(x, y);
                }
            }
            snprintf(buffer, sizeof(buffer), "BOOM%02d%02d\n", boom_x, boom_y);
            UART_SendString(buffer);
            boom_x = (boom_x + 1) % FIELD_SIZE;
            if (boom_x == 0) {
                boom_y = (boom_y + 1) % FIELD_SIZE;
            }
            break;
        case RESULT:
            // Code for RESULT state
            UART_SendString("RESULT\n");
            currentState = GAMEEND;
            break;
        case GAMEEND:
            // Code for GAMEEND state
            UART_SendString("GAMEEND\n");
            currentState = INIT;
            break;
        case STATE_ERROR:
            // Code for ERROR state
            UART_SendString("Error occurred!\n");
            currentState = INIT;
            break;
    }
}
