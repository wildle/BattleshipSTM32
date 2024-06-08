#include "stm32f0xx.h"
#include "mci_clock.h"

void SystemClock_Config(void) {
    // Enable HSE (High-Speed External) oscillator
    RCC->CR |= RCC_CR_HSEON;
    while (!(RCC->CR & RCC_CR_HSERDY));

    // Configure the PLL for 48 MHz operation
    RCC->CFGR |= RCC_CFGR_PLLSRC_HSE_PREDIV;
    RCC->CFGR |= RCC_CFGR_PLLMUL6; // HSE * 6 = 48 MHz

    RCC->CR |= RCC_CR_PLLON;
    while (!(RCC->CR & RCC_CR_PLLRDY));

    // Select PLL as system clock source
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);

    SystemCoreClockUpdate();
}
