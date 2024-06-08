#include "stm32f0xx.h"
#include "mci_clock.h"
#include "uart.h"
#include "state_machine.h"

int main(void) {
    // System Clock Configuration
    SystemClock_Config();

    // Initialize UART
    UART_Init();

    // Initialize State Machine
    StateMachine_Init();

    while (1) {
        // Execute State Machine
        StateMachine_Run();
    }
}
