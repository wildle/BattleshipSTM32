#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "mci_clock.h"
#include "stm32f0xx.h"

#define BUFFER 100
#define BAUDRATE 9600

 // #define DEBUG 1 // Entferne diese Zeile, um den DEBUG-Modus zu deaktivieren

// Enum für die State-Zustände
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

volatile State currentState = INIT;            // Aktueller Zustand der State-Maschine
volatile LOOP currentLoop = LOOP_DECIDE_STATE; // Aktueller Zustand der Loop-Maschine
volatile int isPlayer1 = 0;                    // Variable zur Bestimmung, ob der Spieler Player 1 ist
char rxbuffer[BUFFER] = {0};                   // Empfangspuffer für Nachrichten
volatile int rx_index = 0;                     // Index für den Empfangspuffer
int op_cs[10] = {0};                           // Array zur Speicherung der Gegnerkoordinaten
volatile int def = 0;                          // Defensiv-Flag
int targetx = 9;                               // Zielkoordinate x
int targety = 9;                               // Zielkoordinate y
int op_targetx = 0;                            // Gegnerzielkoordinate x
int op_targety = 0;                            // Gegnerzielkoordinate y
int spielfeld[10][10] = {0};                   // Spielfeldarray

void send_msg(const char *msg);
void calculate_checksum(int spielfeld[10][10], char *checksum);

void ADC_Init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_ADCEN; // ADC-Takt aktivieren
    ADC1->CR |= ADC_CR_ADEN;           // ADC aktivieren
    while (!(ADC1->ISR & ADC_ISR_ADRDY))
        ;                             // Warten, bis ADC bereit ist
    ADC1->CHSELR = ADC_CHSELR_CHSEL0; // Kanal 0 auswählen (falls verwendet)
    ADC1->CFGR1 |= ADC_CFGR1_CONT;    // Kontinuierlicher Modus
    ADC1->CR |= ADC_CR_ADSTART;       // ADC-Start
}

uint16_t ADC_Read(void)
{
    while (!(ADC1->ISR & ADC_ISR_EOC))
        ;            // Warten, bis die Konvertierung abgeschlossen ist
    return ADC1->DR; // Rückgabe des ADC-Wertes
}

void init_spielfeld(int spielfeld[10][10])
{
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            spielfeld[i][j] = 0; // Wasser
        }
    }
}

int can_place_ship(int spielfeld[10][10], int x, int y, int dir, int len)
{
    // Überprüfen, ob das Schiff innerhalb des Spielfelds liegt
    if ((dir == 0 && (x + len) > 10) || (dir == 1 && (y + len) > 10))
    {
        return 0;
    }

    for (int i = 0; i < len; i++)
    {
        int nx = x + (dir == 0 ? i : 0);
        int ny = y + (dir == 1 ? i : 0);

        // Überprüfen, ob das Schiff angrenzende Schiffe berührt
        for (int dx = -1; dx <= 1; dx++)
        {
            for (int dy = -1; dy <= 1; dy++)
            {
                int ax = nx + dx;
                int ay = ny + dy;
                if (ax >= 0 && ax < 10 && ay >= 0 && ay < 10 && spielfeld[ax][ay] != 0)
                {
                    return 0;
                }
            }
        }
    }
    return 1;
}

void place_ship(int spielfeld[10][10], int x, int y, int dir, int len)
{
    for (int i = 0; i < len; i++)
    {
        int nx = x + (dir == 0 ? i : 0);
        int ny = y + (dir == 1 ? i : 0);
        spielfeld[nx][ny] = len; // Schiff platzieren
    }
}

void place_ships(int spielfeld[10][10])
{
    int schiffe[] = {5, 4, 4, 3, 3, 3, 2, 2, 2, 2};

    for (int i = 0; i < sizeof(schiffe) / sizeof(schiffe[0]); i++)
    {
        int ship_len = schiffe[i];
        int placed = 0;

        while (!placed)
        {
            int dir = ADC_Read() % 2; // Zufällige Richtung von ADC-Wert
            int x = ADC_Read() % 10;  // Zufällige x-Position von ADC-Wert
            int y = ADC_Read() % 10;  // Zufällige y-Position von ADC-Wert

            if (can_place_ship(spielfeld, x, y, dir, ship_len))
            {
                place_ship(spielfeld, x, y, dir, ship_len);
                placed = 1;
            }
        }
    }
}

void print_spielfeld(int spielfeld[10][10])
{
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            char buffer[4];
            sprintf(buffer, "%d ", spielfeld[i][j]);
            send_msg(buffer);
        }
        send_msg("\n");
    }
}

void print_debug_info(int spielfeld[10][10])
{
#ifdef DEBUG
    send_msg("DEBUG: Spielfeld:\n");
    print_spielfeld(spielfeld);
    char checksum[12] = "CS";
    calculate_checksum(spielfeld, checksum + 2);
    send_msg("DEBUG: Checksumme: ");
    send_msg(checksum);
#endif
}

void calculate_checksum(int spielfeld[10][10], char *checksum)
{
    for (int i = 0; i < 10; i++)
    {
        int sum = 0;
        for (int j = 0; j < 10; j++)
        {
            if (spielfeld[j][i] != 0)
            {
                sum++;
            }
        }
        checksum[i] = '0' + sum;
    }
    checksum[10] = '\n';
}

void Init(void)
{
    // Initialisierung des Systems
    EPL_SystemClock_Config(); // Konfiguration des Systemtakts
    // Aktivierung der GPIOA- und USART2-Takte
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
    // GPIOA Modus einrichten
    GPIOA->MODER |= GPIO_MODER_MODER2_1;
    GPIOA->AFR[0] |= 0b0001 << (4 * 2);
    // GPIOB Modus einrichten, setze B3, B4, B5 auf Ausgang
    GPIOB->MODER |= GPIO_MODER_MODER3_0;
    GPIOB->MODER |= GPIO_MODER_MODER4_0;
    GPIOB->MODER |= GPIO_MODER_MODER5_0;
    // GPIOB für LED Push/Pull einrichten
    GPIOB->OTYPER &= ~GPIO_OTYPER_OT_3; // BLUE auf B3
    GPIOB->OTYPER &= ~GPIO_OTYPER_OT_4; // GREEN auf B4
    GPIOB->OTYPER &= ~GPIO_OTYPER_OT_5; // RED auf B5
    // GPIOC Modus für Blue Button einrichten
    GPIOC->MODER &= ~GPIO_MODER_MODER13;
    GPIOC->PUPDR |= GPIO_PUPDR_PUPDR13_0;
    // USART2 Modus einrichten
    GPIOA->MODER |= GPIO_MODER_MODER3_1;
    GPIOA->AFR[0] |= 0b0001 << (4 * 3);
    // USART2 einrichten
    USART2->BRR = (APB_FREQ / BAUDRATE);
    USART2->CR1 |= (USART_CR1_RE | USART_CR1_TE | USART_CR1_UE);

    ADC_Init(); // Initialisierung des ADC

    // Zufallsgenerator initialisieren
    srand(time(NULL));

    // Spielfeld initialisierung und Schiffe platzieren
    init_spielfeld(spielfeld);
    place_ships(spielfeld);

    // Spielfeld zur Überprüfung ausdrucken, wenn DEBUG aktiviert ist
    print_debug_info(spielfeld);
}

void clearbuffer()
{
    // Empfangspuffer leeren
    memset(rxbuffer, 0, BUFFER);
    rx_index = 0;
}

void send_msg(const char *msg)
{
    // Nachricht über UART senden
    while (*msg)
    {
        while (!(USART2->ISR & USART_ISR_TXE))
            ; // Warten bis Datenregister leer ist
        {};
        USART2->TDR = *msg++;
    }
}

void send_int(int num)
{
    // Integer als Nachricht senden
    char buffer[10];
    sprintf(buffer, "%d\n", num);
    send_msg(buffer);
}

void settarget()
{
    // Zielkoordinate setzen
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
    // Nachricht über UART empfangen
    if (USART2->ISR & USART_ISR_RXNE)
    {
        char c = (char)(USART2->RDR); // Empfangene Daten lesen
        if (c == '\n')
        {
            rxbuffer[rx_index] = '\0';
            rx_index = 0;
            // Buffer terminieren
        }
        else
        {
            rxbuffer[rx_index] = c;
            rx_index++;
            // Buffer überschreiben
        }
    }
}

int check_game_end(int spielfeld[10][10])
{
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            if (spielfeld[i][j] != 0)
            {
                return 0; // Es gibt noch Schiffe auf dem Spielfeld
            }
        }
    }
    return 1; // Alle Schiffe wurden versenkt
}

int main(void)
{
    int iter = 0;

    // System initialisieren
    Init();

    // Test der UART Übertragung
    // send_int(12345);
    send_msg("SYSTEM_INITIALIZED\n");

    // Hauptschleife
    while (1)
    {
        iter++;

        switch (currentState)
        {
        case INIT:
            // Warten auf Tastendruck (wird im Interrupt gesetzt)
            if (!(GPIOC->IDR & GPIO_IDR_13))
            {
                isPlayer1 = 1;
                currentState = S1_WAIT_CS; // Wechsel in den S1_WAIT_CS Zustand
                send_msg("START52216069\n");
            }

            get_msg();
            if (strncmp(rxbuffer, "START", 5) == 0 && strlen(rxbuffer) >= 9)
            {
                // isPlayer1 = 0;
                // send_msg("CS1234569000\n");
                //  Berechne und sende die Spielfeld-Checksummen-Nachricht
                char checksum[12] = "CS";
                calculate_checksum(spielfeld, checksum + 2);
                send_msg(checksum);

                currentState = S2_WAIT_CS;
                clearbuffer();
            }
            break;

        case S1_WAIT_CS:
            // Warten auf CS Nachricht
            get_msg();
            if (strncmp(rxbuffer, "CS", 2) == 0 && strlen(rxbuffer) >= 12)
            {
                // send_msg("CS1234567890\n");
                char checksum[12] = "CS";
                calculate_checksum(spielfeld, checksum + 2);
                send_msg(checksum);

                for (int i = 0; i < 10; i++)
                {
                    op_cs[i] = rxbuffer[2 + i] - '0';
                }
                clearbuffer();
                currentState = S1_WAIT_START;
            }
            break;

        case S1_WAIT_START:
            // Warten auf START Nachricht
            get_msg();
            if (strncmp(rxbuffer, "START", 5) == 0 && strlen(rxbuffer) >= 9)
            {
                send_msg("start received\n");
                currentState = PLAY;
                clearbuffer();
            }
            break;

        case S2_WAIT_CS:
            // Warten auf CS Nachricht
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
            // Hauptspielzustand
            switch (currentLoop)
            {
            case LOOP_DECIDE_STATE:
                // Entscheidungsschleife
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
                // Schusslogik
                settarget();
                // Schussnachricht senden
                char hitmsg[12];
                sprintf(hitmsg, "BOOM%d%d\n", targetx, targety);
                send_msg(hitmsg);
                clearbuffer();
                currentLoop = LOOP_EV_SHOT;
                break;
            case LOOP_GETSHOT:
                // Schuss empfangen
                get_msg();
                if (strncmp(rxbuffer, "BOOM", 4) == 0 && strlen(rxbuffer) >= 6)
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
                // Schuss evaluieren
                get_msg();
                if (strncmp(rxbuffer, "T", 1) == 0)
                {
                    // Treffer
                    clearbuffer();
                    currentLoop = LOOP_DECIDE_STATE;
                }
                else if (strncmp(rxbuffer, "W", 1) == 0)
                {
                    // Wasser (verfehlt)
                    clearbuffer();
                    currentLoop = LOOP_DECIDE_STATE;
                }
                else if (strncmp(rxbuffer, "S", 1) == 0)
                {
                    // Spielfeldnachricht
                    clearbuffer();
                    currentState = GAMEEND;
                }
                break;
            }
            break;

        case GAMEEND:


            for (int i = 0; i < 10; i++)
            {
                char buffer[14]; // Ausreichend groß, um die gesamte Zeile zu speichern
                char *ptr = buffer;
                ptr += sprintf(ptr, "SF%dD", i);
                for (int j = 0; j < 10; j++)
                {
                    ptr += sprintf(ptr, "%d", spielfeld[i][j]);
                }
                send_msg(buffer);
                send_msg("\n");
            }

            currentState = INIT;
            break;
        }
    }
}
