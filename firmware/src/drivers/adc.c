//  ***************************************************************************
/// @file    adc.c
/// @author  NeoProg
//  ***************************************************************************
#include "adc.h"
#include "project_base.h"
#include "systimer.h"
#define ADC_INPUT_1_PIN                 (0) // PA0 ADC CH0


//  ***************************************************************************
/// @brief  ADC initialization
/// @param  none
/// @return none
//  ***************************************************************************
void adc_init(void) {
    
    // Setup analog input (PA0)
    gpio_set_mode        (GPIOA, ADC_INPUT_1_PIN, GPIO_MODE_ANALOG);
    gpio_set_output_speed(GPIOA, ADC_INPUT_1_PIN, GPIO_SPEED_HIGH);
    gpio_set_pull        (GPIOA, ADC_INPUT_1_PIN, GPIO_PULL_NO);

    // Setup ADC
    RCC->APB2RSTR |= RCC_APB2RSTR_ADCRST;
    RCC->APB2RSTR &= ~RCC_APB2RSTR_ADC1RST;
    ADC1->CFGR2 |= ADC_CFGR2_CKMODE_1;
    ADC1->CR |= ADC_CR_ADCAL;
    while (ADC1->CR & ADC_CR_ADCAL);
    ADC1->DR;
    ADC1->CR |= ADC_CR_ADEN;
    ADC1->SMPR |= ADC_SMPR1_SMPR_2 | ADC_SMPR1_SMPR_1 | ADC_SMPR1_SMPR_0;
    ADC1->CHSELR = ADC_CHSELR_CHSEL0;
}

//  ***************************************************************************
/// @brief  Start conversion
/// @param  none
/// @return none
//  ***************************************************************************
void adc_start_conversion(void) {
    ADC1->CR |= ADC_CR_ADSTART;
}

//  ***************************************************************************
/// @brief  Check conversion complete
/// @param  none
/// @return true - success, false - in progress
//  ***************************************************************************
bool adc_is_conversion_complete(void) {
    return ADC1->ISR & ADC_ISR_ADRDY;
}

//  ***************************************************************************
/// @brief  Get conversion result
/// @param  channel: channel index
/// @return conversion result
//  ***************************************************************************
uint16_t adc_get_conversion_result(void) {
    return ADC1->DR;
}
