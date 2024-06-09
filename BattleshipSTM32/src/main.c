#include "main.h"
#include "state_machine.h"

void SystemClock_Config(void);

int main(void) {
    // Initialize system clock
    SystemClock_Config();

    // Initialize UART
    UART_Init();

    // Initialize State Machine
    StateMachine_Init();

    // Test UART transmission
    UART_SendString("Hello, UART!\n");

    // Main loop
    while (1) {
        // Run the state machine
        StateMachine_Run();
        // Add a small delay to allow UART interrupts to be handled
        for (volatile int i = 0; i < 10000; ++i);
    }
}

void SystemClock_Config(void) {
    // Configure the system clock here (already implemented in mci_clock.c)
}
