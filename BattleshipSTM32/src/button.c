#include "button.h"
#include "state_machine.h"
#include "uart.h" // Include UART for debugging
#include "stm32f0xx.h"

void Button_Init(void) {
    // Enable clock for GPIOC (B1 is connected to PC13)
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;

    // Set PC13 as input
    GPIOC->MODER &= ~(GPIO_MODER_MODER13);

    // Configure PC13 as external interrupt source
    SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI13_PC;

    // Enable interrupt on falling edge for EXTI13
    EXTI->FTSR |= EXTI_FTSR_TR13;
    EXTI->IMR |= EXTI_IMR_MR13;

    // Enable EXTI line 13 interrupt
    NVIC_SetPriority(EXTI4_15_IRQn, 1);
    NVIC_EnableIRQ(EXTI4_15_IRQn);
}

// Simple debouncing function
void debounce(void) {
    for (volatile int i = 0; i < 100000; i++);
}

// EXTI line 13 interrupt handler (for PC13)
void EXTI4_15_IRQHandler(void) {
    if (EXTI->PR & EXTI_PR_PR13) {
        debounce(); // Debounce the button press
        EXTI->PR |= EXTI_PR_PR13; // Clear the interrupt flag

        if (!(GPIOC->IDR & GPIO_IDR_13)) { // Check if button is still pressed
            UART_SendString("Button Pressed!\n"); // Debugging message
            // Call the function to change the state
            StateMachine_SetState(START_S1);
        }
    }
}
