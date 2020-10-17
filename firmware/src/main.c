#include "gate_pwm.h"
#include "clocks_cfg.h"

#include "stm32f0xx.h"
#include <stdbool.h>
#include <stdint.h>

#define DEBUG_PIN                       (10)


static void system_clock_setup(void);
static void systimer_setup(void);


int main() {
    
    system_clock_setup();
    systimer_setup();
  
    // Enable GPIO clocks
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    
    /*GPIOA->MODER   |= (1 << (DEBUG_PIN * 2));   // Set GPIO as output
    GPIOA->OTYPER  |= (0 << (DEBUG_PIN * 1));   // Push-pull output
    GPIOA->OSPEEDR |= (3 << (DEBUG_PIN * 2));   // Set GPIO high speed
    GPIOA->PUPDR   |= (0 << (DEBUG_PIN * 2));   // No pull*/
    
    
    gate_pwm_init();
    gate_pwm_enable();
    
    while (true) {
        
        // Check HSE failure
        /*if (RCC->CIR & RCC_CIR_CSSF) {
            gate_pwm_disable();
        }*/
        
        asm("NOP");
    }
}

static void system_clock_setup(void) {
    
    // Enable HSE
    RCC->CR |= RCC_CR_HSEON | RCC_CR_CSSON;
    for (uint32_t i = 0; i < UINT16_MAX; ++i) {
        asm("NOP");
    }
    
    // Select HSI as System clocks
    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_HSI;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI);
    
    // Disable PLL
    RCC->CR &= ~RCC_CR_PLLON;
    while((RCC->CR & RCC_CR_PLLRDY) != 0);
    
    // Select PLL clock source
    if (RCC->CR & RCC_CR_HSERDY) {
        RCC->CFGR = RCC_CFGR_PLLMUL6 | RCC_CFGR_PLLSRC_HSE_PREDIV;
    }
    else {
        RCC->CFGR = RCC_CFGR_PLLMUL12 | RCC_CFGR_PLLSRC_HSI_DIV2;
    }
    
    // Enable PLL
    RCC->CR |= RCC_CR_PLLON;
    while((RCC->CR & RCC_CR_PLLRDY) == 0);
    
    // Select PLL as System clocks
    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);
}

static void systimer_setup(void) {
    
    // Systimer setup 
    SysTick->VAL = 0;
    SysTick->LOAD = SYSTEM_CORE_CLOCK / 1000;
    SysTick->CTRL = (1 << SysTick_CTRL_TICKINT_Pos) | (1 << SysTick_CTRL_CLKSOURCE_Pos) | (1 << SysTick_CTRL_ENABLE_Pos);
    NVIC_EnableIRQ(SysTick_IRQn);
}

void SysTick_Handler(void) {
  
    //GPIOA->ODR ^= (1 << DEBUG_PIN);
  
    asm("NOP");
}