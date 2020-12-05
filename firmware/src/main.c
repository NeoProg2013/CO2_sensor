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
#define MEAS_PERIOD_MS                  (3000)
#define WARMUP_MEAS_COUNT               (5)


static void system_init(void);



//  ***************************************************************************
/// @brief  Program entry point
/// @param  none
/// @return none
//  ***************************************************************************
int main() {
    
    system_init();
    systimer_init();
    if (!display_init()) {
        NVIC_SystemReset();
        return 0;
    }
    led_init();
    sysmon_init();
    co2_sensor_init();
    
    bool is_warmup = true;
    uint32_t meas_counter = 0;
    while (true) {
        static uint64_t last_meas_time = 0;
        if (get_time_ms() - last_meas_time < MEAS_PERIOD_MS) {
            systimer_change_step(1000);
            __WFI();
            continue;
        }
        systimer_change_step(1);
        
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
        if (!is_warmup) {
            static uint32_t conc_errors_count = 0;
            if (co2_sensor_read_concentration()) {
                concentration = co2_sensor_get_concentration();
                conc_errors_count = 0;
            } else {
                ++conc_errors_count;
                if (conc_errors_count > 10) {
                    concentration = CONC_UNKNOWN_VALUE;
                }
            }
        } else {
            if (++meas_counter >= WARMUP_MEAS_COUNT) {
                is_warmup = false;
            }
        }
        
        // Update display
        if (!display_update(concentration, battery_charge)) {
            NVIC_SystemReset();
            return 0;
        }
        last_meas_time = get_time_ms();
        
        // Update indication
        if (concentration == CONC_UNKNOWN_VALUE) {
            led_set_state(STATE_NORMAL);
        } else if (concentration > 2500) {
            led_set_state(STATE_ALARM);
        } else if (concentration > 1700) {
            led_set_state(STATE_WARNING);
        } else {
            led_set_state(STATE_NORMAL);
        }
        led_process();
        
        last_meas_time = get_time_ms();
    }
}

//  ***************************************************************************
/// @brief  System initialization
/// @param  none
/// @return none
//  ***************************************************************************
static void system_init(void) {
    
    // Enable Prefetch Buffer
    FLASH->ACR = FLASH_ACR_PRFTBE;
    
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
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
}
