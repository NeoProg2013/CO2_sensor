//  ***************************************************************************
/// @file    led.c
/// @author  NeoProg
//  ***************************************************************************
#include "led.h"
#include "project_base.h"
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
    GPIOF->BRR = (0x01 << RED_LED_PIN);
    gpio_set_mode        (GPIOF, RED_LED_PIN, GPIO_MODE_OUTPUT);
    gpio_set_output_type (GPIOF, RED_LED_PIN, GPIO_TYPE_PUSH_PULL);
    gpio_set_output_speed(GPIOF, RED_LED_PIN, GPIO_SPEED_LOW);
    gpio_set_pull        (GPIOF, RED_LED_PIN, GPIO_PULL_NO);
    
    // Setup yellow led control pin
    GPIOF->BRR = (0x01 << YELLOW_LED_PIN);
    gpio_set_mode        (GPIOF, YELLOW_LED_PIN, GPIO_MODE_OUTPUT);
    gpio_set_output_type (GPIOF, YELLOW_LED_PIN, GPIO_TYPE_PUSH_PULL);
    gpio_set_output_speed(GPIOF, YELLOW_LED_PIN, GPIO_SPEED_LOW);
    gpio_set_pull        (GPIOF, YELLOW_LED_PIN, GPIO_PULL_NO);
    
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
    
    for (uint32_t i = 0; i < 3; ++i) { 
        if (led_state == STATE_ALARM) {
            GPIOF->BSRR = (0x01 << RED_LED_PIN);
        } else if (led_state == STATE_WARNING) {
            GPIOF->BSRR = (0x01 << YELLOW_LED_PIN);
        }
        delay_ms(300);
        GPIOF->BRR = (0x01 << RED_LED_PIN) | (0x01 << YELLOW_LED_PIN);
        delay_ms(300);
    }
}
