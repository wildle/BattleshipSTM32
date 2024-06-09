#include "main.h"
#include "state_machine.h"
#include "button.h"

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

        // Add a delay to avoid busy-waiting loop
        for (volatile int i = 0; i < 100000; i++); // Simple delay
    }
}

void SystemClock_Config(void) {
    // Configure the system clock here (already implemented in mci_clock.c)
}
