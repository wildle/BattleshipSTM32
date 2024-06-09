#include "main.h"

void SystemClock_Config(void);

int main(void) {
    // Initialize system clock
    SystemClock_Config();

    // Initialize UART
    UART_Init();

    // Test UART transmission
    UART_SendString("Hello, UART!\n");

    // Main loop
    while (1) {
        // Your main program logic
    }
}

void SystemClock_Config(void) {
    // Configure the system clock here (already implemented in mci_clock.c)
}
