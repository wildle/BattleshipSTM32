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
        for (volatile int i = 0; i < 100000; i++);  // Simple delay to prevent spamming messages
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
    // Configure the system clock to 48 MHz
    RCC->CR |= RCC_CR_HSEON; // Enable HSE
    while (!(RCC->CR & RCC_CR_HSERDY)); // Wait until HSE is ready

    // Configure PLL
    RCC->CFGR |= RCC_CFGR_PLLSRC_HSE_PREDIV; // HSE as PLL source
    RCC->CFGR |= RCC_CFGR_PLLMUL6; // PLL multiplier 6

    RCC->CR |= RCC_CR_PLLON; // Enable PLL
    while (!(RCC->CR & RCC_CR_PLLRDY)); // Wait until PLL is ready

    RCC->CFGR |= RCC_CFGR_SW_PLL; // Select PLL as system clock source
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL); // Wait until PLL is system clock source
}
