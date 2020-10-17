//  ***************************************************************************
/// @file    gate_pwm.c
/// @author  NeoProg
//  ***************************************************************************
#include "gate_pwm.h"

#include "stm32f0xx.h"
#include "clocks_cfg.h"
#include <stdint.h>


#define MOSFET_SWITCH_FREQUENCY             (100000)
#define MOSFET_LOW_SIDE_GATE                (10)    // TIM1 CH3
#define MOSFET_HIGH_SIDE_GATE               (9)     // TIM1 CH2


void gate_pwm_init(void) {
    
    // PWM timer reset and enable clocks
    RCC->APB2RSTR |= RCC_APB2RSTR_TIM1RST;
    RCC->APB2RSTR &= ~RCC_APB2RSTR_TIM1RST;
    RCC->APB2ENR  |= RCC_APB2ENR_TIM1EN;
    
    // Calculate PWM duty cycle resolution
    uint32_t resolution = SYSTEM_CORE_CLOCK / MOSFET_SWITCH_FREQUENCY;
    resolution /= 2; // For PWM center align
    
    // PWM timer initialization: PWM mode, CH2(high side) and CH3(low side), center align, CH3 - inverted
    TIM1->CR1   = (1 << TIM_CR1_CMS_Pos);
    TIM1->BDTR |= TIM_BDTR_MOE;
    TIM1->CCMR1 = (6 << TIM_CCMR1_OC2M_Pos);
    TIM1->CCMR2 = (6 << TIM_CCMR2_OC3M_Pos);
    TIM1->PSC   = 0;
    TIM1->ARR   = resolution;
    TIM1->CCR2  = resolution / 2;
    TIM1->CCR3  = resolution / 3;
    TIM1->CCER  = TIM_CCER_CC2E | TIM_CCER_CC2P | TIM_CCER_CC3E;
    
    GPIOA->ODR     &= ~(1 << MOSFET_HIGH_SIDE_GATE);
    GPIOA->MODER   |= (2 << (MOSFET_HIGH_SIDE_GATE * 2));           // Set GPIO as output
    GPIOA->OTYPER  |= (0 << (MOSFET_HIGH_SIDE_GATE * 1));           // Push-pull output
    GPIOA->OSPEEDR |= (3 << (MOSFET_HIGH_SIDE_GATE * 2));           // Set GPIO high speed
    GPIOA->PUPDR   |= (0 << (MOSFET_HIGH_SIDE_GATE * 2));           // No pull
    GPIOA->AFR[1]  |= (2 << ((MOSFET_HIGH_SIDE_GATE - 8) * 4));     // Select AF

    GPIOA->ODR     &= ~(1 << MOSFET_LOW_SIDE_GATE);
    GPIOA->MODER   |= (2 << (MOSFET_LOW_SIDE_GATE * 2));           // Set GPIO as output
    GPIOA->OTYPER  |= (0 << (MOSFET_LOW_SIDE_GATE * 1));           // Push-pull output
    GPIOA->OSPEEDR |= (3 << (MOSFET_LOW_SIDE_GATE * 2));           // Set GPIO high speed
    GPIOA->PUPDR   |= (0 << (MOSFET_LOW_SIDE_GATE * 2));           // No pull
    GPIOA->AFR[1]  |= (2 << ((MOSFET_LOW_SIDE_GATE - 8) * 4));     // Select AF
}

void gate_pwm_disable(void) {
    
    TIM1->CR1 &= ~TIM_CR1_CEN;
}

void gate_pwm_enable(void) {
    
    TIM1->EGR  = TIM_EGR_UG;
    TIM1->CR1 |= TIM_CR1_CEN;
}

void gate_pwm_set_duty_cycle(uint16_t duty_cycle) {
 
    TIM1->CCR2 = duty_cycle;
    TIM1->CCR3 = duty_cycle;
}
