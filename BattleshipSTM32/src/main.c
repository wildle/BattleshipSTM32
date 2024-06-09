#include "main.h"

void SystemClock_Config(void);

int main(void) {
    // Initialize system clock
    SystemClock_Config();

    // Initialize UART
    UART_Init();

    // Initialize Button
    Button_Init();

    // Initialize State Machine
    StateMachine_Init();

    // Test UART transmission
    UART_SendString("Hello, UART!\n");

    // Main loop
    while (1) {
        // Run the state machine
        StateMachine_Run();
    }
}

void SystemClock_Config(void) {
    // Configure the system clock here (already implemented in mci_clock.c)
}

void Button_Init(void) {
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN; // Enable clock for GPIOC

    // Configure PC13 (Blue Push Button) as input
    GPIOC->MODER &= ~(GPIO_MODER_MODER13);

    // Configure PC13 as external interrupt
    SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI13_PC;
    EXTI->IMR |= EXTI_IMR_IM13;
    EXTI->RTSR |= EXTI_RTSR_RT13;
    NVIC_SetPriority(EXTI4_15_IRQn, 2);
    NVIC_EnableIRQ(EXTI4_15_IRQn);
}

void EXTI4_15_IRQHandler(void) {
    if (EXTI->PR & EXTI_PR_PR13) {
        EXTI->PR |= EXTI_PR_PR13; // Clear interrupt flag
        // Call a function to handle the button press
        handle_button_press();
    }
}
