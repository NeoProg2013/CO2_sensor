//  ***************************************************************************
/// @file    led.c
/// @author  NeoProg
//  ***************************************************************************
#include "project_base.h"
#include "led.h"
#include "systimer.h"
#define RED_LED_PIN                 (1)
#define YELLOW_LED_PIN              (0)


static led_state_t led_state;


//  ***************************************************************************
/// @brief  LED initialization
/// @param  none
/// @return none
//  ***************************************************************************
void led_init(void) {
    // Setup red led control pin
    GPIOF->BRR      =  (0x01 << RED_LED_PIN);
    GPIOF->MODER   |=  (0x01u << (RED_LED_PIN * 2u)); // Output mode
    GPIOF->OSPEEDR |=  (0x03u << (RED_LED_PIN * 2u)); // High speed
    GPIOF->PUPDR   &= ~(0x03u << (RED_LED_PIN * 2u)); // Disable pull
    
    // Setup yellow led control pin
    GPIOF->BRR      =  (0x01 << YELLOW_LED_PIN);
    GPIOF->MODER   |=  (0x01u << (YELLOW_LED_PIN * 2u)); // Output mode
    GPIOF->OSPEEDR |=  (0x03u << (YELLOW_LED_PIN * 2u)); // High speed
    GPIOF->PUPDR   &= ~(0x03u << (YELLOW_LED_PIN * 2u)); // Disable pull
    
    // Blink
    GPIOF->BSRR = (0x01 << RED_LED_PIN) | (0x01 << YELLOW_LED_PIN);
    delay_ms(500);
    GPIOF->BRR = (0x01 << RED_LED_PIN) | (0x01 << YELLOW_LED_PIN);
    
    // Set default state
    led_state = STATE_NORMAL;
}

//  ***************************************************************************
/// @brief  LED set state
/// @param  state: new state
/// @return none
//  ***************************************************************************
void led_set_state(led_state_t state) {
    led_state = state;
}

//  ***************************************************************************
/// @brief  LED process
/// @note   Call from main loop
/// @return none
//  ***************************************************************************
void led_process(void) {
    
    static uint64_t start_time = 0;
    if (get_time_ms() - start_time > 250) {
        
        if (led_state == STATE_ALARM) {
            GPIOF->BRR = (0x01 << YELLOW_LED_PIN);
            GPIOF->ODR ^= (0x01 << RED_LED_PIN);
        } 
        else if (led_state == STATE_WARNING) {
            GPIOF->BRR = (0x01 << RED_LED_PIN);
            GPIOF->ODR ^= (0x01 << YELLOW_LED_PIN);
        }
        else {
            GPIOF->BRR = (0x01 << RED_LED_PIN) | (0x01 << YELLOW_LED_PIN);
        }
        start_time = get_time_ms();
    }
}
