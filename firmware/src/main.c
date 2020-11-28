//  ***************************************************************************
/// @file    main.c
/// @author  NeoProg
//  ***************************************************************************
#include "project_base.h"
#include "systimer.h"
#include "co2_sensor.h"
#include "led.h"
#include "system_monitor.h"
#include "display.h"
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
    sysmon_init();
    co2_sensor_init();
    if (!display_init()) {
        NVIC_SystemReset();
        return 0;
    }
    
    while (true) {
        static uint64_t last_meas_time = 0;
        if (get_time_ms() - last_meas_time > MEAS_PERIOD_MS) {
        
            // Meas battery charge
            uint8_t battery_charge = CHARGE_UNKNOWN_VALUE;
            static uint32_t charge_errors_count = 0;
            if (sysmon_calc_battery_voltage()) {
                battery_charge = sysmon_get_battery_charge();
                charge_errors_count = 0;
            } else {
                ++charge_errors_count;
                if (charge_errors_count > 10) {
                    battery_charge = CHARGE_UNKNOWN_VALUE;
                }
            }
            
            // Check charger connect
            if (sysmon_is_charger_connect()) {
                battery_charge = CHARGE_UNKNOWN_VALUE;
            }
            
            // Meas concentration
            uint16_t concentration = CONC_UNKNOWN_VALUE;
            static uint32_t conc_errors_count = 0;
            if (co2_sensor_read_concentration()) {
                concentration = co2_sensor_get_concentration();
                conc_errors_count = 0;
                
                // Check thresholds
                if (concentration > 2500) {
                    led_set_state(STATE_ALARM);
                } else if (concentration > 1000) {
                    led_set_state(STATE_WARNING);
                } else {
                    led_set_state(STATE_NORMAL);
                }
            } else {
                ++conc_errors_count;
                if (conc_errors_count > 10) {
                    concentration = CONC_UNKNOWN_VALUE;
                }
            }
            
            // Update display
            if (!display_update(concentration, battery_charge)) {
                NVIC_SystemReset();
                return 0;
            }
            last_meas_time = get_time_ms();
        }
        led_process();
    }
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
    
    // Enable ADC clocks
    RCC->APB2ENR  |= RCC_APB2ENR_ADC1EN;
}
