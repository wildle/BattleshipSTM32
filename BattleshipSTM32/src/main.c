#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mci_clock.h"
#include "stm32f0xx.h"

#define BUFFER 100
#define BAUDRATE 9600

// Enum für die Zustände
typedef enum
{
    INIT,
    S1_WAIT_CS,
    S1_WAIT_START,
    S2_WAIT_CS,
    PLAY,
    GAMEEND
} State;

typedef enum
{
    LOOP_DECIDE_STATE,
    LOOP_SHOOT,
    LOOP_GETSHOT,
    LOOP_EV_SHOT
} LOOP;

volatile State currentState = INIT;
volatile LOOP currentLoop = LOOP_DECIDE_STATE;
volatile int isPlayer1 = 0;
char rxbuffer[BUFFER] = {0};
volatile int rx_index = 0;
int op_cs[10] = {0};
volatile int def = 0;
int targetx = 9;
int targety = 9;
int op_targetx = 0;
int op_targety = 0;
int spielfeld[10][10] = {0};

void Init(void)
{
    // Initialization of the system
    EPL_SystemClock_Config();
    // Enable GPIOA and USART2 clocks
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
    // Setup GPIOA Mode
    GPIOA->MODER |= GPIO_MODER_MODER2_1;
    GPIOA->AFR[0] |= 0b0001 << (4 * 2);
    // Setup GPIOB Mode, set B3, B4, B5 to output
    GPIOB->MODER |= GPIO_MODER_MODER3_0;
    GPIOB->MODER |= GPIO_MODER_MODER4_0;
    GPIOB->MODER |= GPIO_MODER_MODER5_0;
    // Setup B ports for LED push/pull
    GPIOB->OTYPER &= ~GPIO_OTYPER_OT_3; // BLUE on B3
    GPIOB->OTYPER &= ~GPIO_OTYPER_OT_4; // GREEN on B4
    GPIOB->OTYPER &= ~GPIO_OTYPER_OT_5; // RED on B5
    // Setup GPIOC Mode for Blue button
    GPIOC->MODER &= ~GPIO_MODER_MODER13;
    GPIOC->PUPDR |= GPIO_PUPDR_PUPDR13_0;
    // Setup USART2 Mode
    GPIOA->MODER |= GPIO_MODER_MODER3_1;
    GPIOA->AFR[0] |= 0b0001 << (4 * 3);
    // Setup USART2
    USART2->BRR = (APB_FREQ / BAUDRATE);
    USART2->CR1 |= (USART_CR1_RE | USART_CR1_TE | USART_CR1_UE);
}

void clearbuffer()
{
    memset(rxbuffer, 0, BUFFER);
    rx_index = 0;
}

void send_msg(const char *msg)
{
    // Interrupt style
    while (*msg)
    {
        while (!(USART2->ISR & USART_ISR_TXE))
            ; // Wait until transmit data register is empty
        {};
        USART2->TDR = *msg++;
    }
}

void send_int(int num)
{
    char buffer[10];
    sprintf(buffer, "%d\n", num);
    send_msg(buffer);
}

void settarget()
{
    targetx++;
    if (targetx > 9)
    {
        targetx = 0;
        targety++;
        if (targety > 9)
        {
            targety = 0;
            targetx = 0;
        }
    }
}

void get_msg()
{
    if (USART2->ISR & USART_ISR_RXNE)
    {
        char c = (char)(USART2->RDR); // Read received data
        if (c == '\n')
        {
            rxbuffer[rx_index] = '\0';
            rx_index = 0;
            // terminate buffer
        }
        else
        {
            rxbuffer[rx_index] = c;
            rx_index++;
            // overwrite buffer
        }
    }
}

int main(void)
{
    int iter = 0;

    // Initialize System
    Init();

    // Test UART transmission
    // send_int(12345);
    send_msg("SYSTEM_INITIALIZED\n");

    // Main loop
    while (1)
    {
        iter++;

        switch (currentState)
        {
        case INIT:
            // Warte auf Tasterdruck (wird im Interrupt gesetzt)
            if (!(GPIOC->IDR & GPIO_IDR_13))
            {
                isPlayer1 = 1;
                currentState = S1_WAIT_CS; // Wechsel in den PLAY Zustand
                send_msg("START52216069\n");
            }

            get_msg();
            if (strncmp(rxbuffer, "START", 5) == 0 && strlen(rxbuffer) >= 9)
            {
                isPlayer1 = 0;
                send_msg("CS1234569000\n");
                currentState = S2_WAIT_CS;
                clearbuffer();
            }
            break;

        case S1_WAIT_CS:
            // Implementiere Logik für START_S2
            get_msg();
            if (strncmp(rxbuffer, "CS", 2) == 0 && strlen(rxbuffer) >= 12)
            {
                send_msg("CS1234567890\n");
                for (int i = 0; i < 10; i++)
                {
                    op_cs[i] = rxbuffer[2 + i] - '0';
                }
                clearbuffer();
                currentState = S1_WAIT_START;
            }
            break;
        case S1_WAIT_START:
            get_msg();
            if (strncmp(rxbuffer, "START", 5) == 0 && strlen(rxbuffer) >= 9)
            {
                send_msg("start received\n");
                currentState = PLAY;
                clearbuffer();
            }
            break;
        case S2_WAIT_CS:
            get_msg();
            if (strncmp(rxbuffer, "CS", 2) == 0 && strlen(rxbuffer) >= 12)
            {
                send_msg("START52216069\n");
                for (int i = 0; i < 10; i++)
                {
                    op_cs[i] = rxbuffer[2 + i] - '0';
                }
                clearbuffer();
                currentState = PLAY;
            }
            break;
        case PLAY:
            // Spielimplementierungen hier
            switch (currentLoop)
            {
            case LOOP_DECIDE_STATE:
                if (isPlayer1 == 1 && def == 0)
                {
                    def = 1;
                    currentLoop = LOOP_SHOOT;
                }
                else if (isPlayer1 == 1 && def == 1)
                {
                    def = 0;
                    currentLoop = LOOP_GETSHOT;
                }
                else if (isPlayer1 == 0 && def == 0)
                {
                    def = 1;
                    currentLoop = LOOP_GETSHOT;
                }
                else if (isPlayer1 == 0 && def == 1)
                {
                    def = 0;
                    currentLoop = LOOP_SHOOT;
                }
                break;

            case LOOP_SHOOT:
                settarget();
                // Implementiere Logik für Schuss
                char hitmsg[12];
                sprintf(hitmsg, "BOOM%d%d\n", targetx, targety);
                send_msg(hitmsg);
                clearbuffer();
                currentLoop = LOOP_EV_SHOT;
                break;
            case LOOP_GETSHOT:
                // Implementiere Logik für Schuss
                get_msg();
                if (strncmp(rxbuffer, "BOOM", 4) == 0 && strlen(rxbuffer) >=6)
                {
                    op_targetx = rxbuffer[4] - '0';
                    op_targety = rxbuffer[5] - '0';
                    if (spielfeld[op_targetx][op_targety] != 0)
                    {
                        send_msg("T\n");
                    }
                    else
                    {
                        send_msg("W\n");
                    }
                    clearbuffer();
                    currentLoop = LOOP_DECIDE_STATE;
                }
                break;
            case LOOP_EV_SHOT:
                // evaluiere Schuss
                get_msg();
                if (strncmp(rxbuffer, "T", 1) == 0)
                {
                    //send_msg("TREFFER\n");
                    clearbuffer();
                    currentLoop = LOOP_DECIDE_STATE;
                }
                else if (strncmp(rxbuffer, "W", 1) == 0)
                {
                    //send_msg("WASSER\n");
                    clearbuffer();
                    currentLoop = LOOP_DECIDE_STATE;
                }
                else if (strncmp(rxbuffer, "S", 1) == 0)
                {
                    //send_msg("SPIELFELDNACHRICHT\n");
                    clearbuffer();
                    currentState = GAMEEND;
                }
                break;
            }
            break;
        case GAMEEND:
            // Spielende Logik hier
            send_msg("SPIEL BEENDET\n");
            break;
        }
    }
}
