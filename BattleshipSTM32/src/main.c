#include "stm32f0xx.h"
#include "mci_clock.h"
#include "uart.h"
#include "state_machine.h"

void Button_Init(void);
void SystemClock_Config(void);

int main(void) {
    // System Clock Configuration
    SystemClock_Config();

    // Initialize UART
    UART_Init();

    // Initialize Button
    Button_Init();

    // Initialize State Machine
    StateMachine_Init();

    while (1) {
        // Execute State Machine
        StateMachine_Run();
    }
}

void Button_Init(void) {
    // Enable GPIOC clock
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;

    // Set PC13 as input
    GPIOC->MODER &= ~GPIO_MODER_MODER13;

    // Enable pull-up resistor
    GPIOC->PUPDR |= GPIO_PUPDR_PUPDR13_0;
}

void SystemClock_Config(void) {
    // Implementiere hier die System Clock Konfiguration
}
