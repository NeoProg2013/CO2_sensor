//  ***************************************************************************
/// @file    main.c
/// @author  NeoProg
//  ***************************************************************************
#include "project_base.h"
#include "systimer.h"
#include "co2_sensor.h"
#include "oled_gl.h"
#include "ssd1306_128x64.h"
#include "led.h"
#define MEAS_PERIOD_MS                  (2000)


static void system_init(void);



//  ***************************************************************************
/// @brief  Program entry point
/// @param  none
/// @return none
//  ***************************************************************************
int main() {
    
    system_init();
    systimer_init();
    led_init();
    
    
    //
    // Display initialization
    //
    if (!oled_gl_init()) {
        NVIC_SystemReset();
        return 0;
    }
    
    oled_gl_draw_string(1, 0, "CO2 sensor", FONT_ID_6x8);
    oled_gl_draw_string(1, 105, "99%", FONT_ID_6x8);
    oled_gl_draw_horizontal_line(2, 0, 7, 128);
    oled_gl_draw_string(4, 8, "----", FONT_ID_24x32);
    oled_gl_draw_string(7, 105, "PPM", FONT_ID_6x8);
    
    if (!oled_gl_display_update()) {
        NVIC_SystemReset();
        return 0;
    }
    
    //
    // Measurements initialization
    //
    co2_sensor_init();
    
    
    //
    // Main loop
    //
    uint64_t last_meas_time = 0;
    uint32_t errors_count = 0;
    bool is_meas_run = true;
    while (is_meas_run) {
        
        if (get_time_ms() - last_meas_time > MEAS_PERIOD_MS) {
            if (errors_count > 10) {
                oled_gl_draw_string(4, 8, "----", FONT_ID_24x32);
                is_meas_run = false;
                continue;
            } 
           
            // Read concentration
            int32_t concentration = co2_sensor_read_concentration();
            if (concentration > 9999) { 
                concentration = 9999;
            }
            
            // Process
            if (concentration > 0) {
                
                // Print concentration value to display
                char conc_str[5] = {0};
                sprintf(conc_str, "%4d", concentration);
                oled_gl_draw_string(1, 105, "99%", FONT_ID_6x8);
                oled_gl_draw_string(4, 8, conc_str, FONT_ID_24x32);
                oled_gl_draw_string(7, 105, "PPM", FONT_ID_6x8);
                if (!oled_gl_display_update()) {
                    NVIC_SystemReset();
                    return 0;
                }
                
                // Check thresholds
                if (concentration > 2500) {
                    led_set_state(STATE_ALARM);
                } else if (concentration > 1000) {
                    led_set_state(STATE_WARNING);
                } else {
                    led_set_state(STATE_NORMAL);
                }
                
                // Reset errors count
                errors_count = 0;
            }
            else {
                ++errors_count;
            }
            
            last_meas_time = get_time_ms();
        }
        led_process();
    }
    
    while (true);
}

//  ***************************************************************************
/// @brief  System initialization
/// @param  none
/// @return none
//  ***************************************************************************
static void system_init(void) {
    
    // Enable Prefetch Buffer and set Flash Latency
    FLASH->ACR = FLASH_ACR_PRFTBE | FLASH_ACR_LATENCY;
    
    // Configure and enable PLL
    RCC->CFGR = RCC_CFGR_PLLMUL12 | RCC_CFGR_PLLSRC_HSI_DIV2;
    RCC->CR |= RCC_CR_PLLON;
    while((RCC->CR & RCC_CR_PLLRDY) != RCC_CR_PLLRDY);
    
    // Switch system clocks to PLL
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);
    
    // Switch USARTx clock source to system clock
    RCC->CFGR3 |= RCC_CFGR3_USART1SW_0;
    
    
    
    // Enable GPIO clocks
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    RCC->AHBENR |= RCC_AHBENR_GPIOFEN;
    
    // Enable clocks for USART1
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
    while ((RCC->APB2ENR & RCC_APB2ENR_USART1EN) == 0);
    
    // Enable clocks for I2C1
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
    while ((RCC->APB1ENR & RCC_APB1ENR_I2C1EN) == 0);
}
