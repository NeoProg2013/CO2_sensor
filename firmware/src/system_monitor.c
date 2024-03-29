//  ***************************************************************************
/// @file    system_status.c
/// @author  NeoProg
//  ***************************************************************************
#include "system_monitor.h"
#include "project_base.h"
#include "adc.h"
#include "systimer.h"
#define CHARGER_DETECTION_PIN           (4) // PA4
#define PAUSE_BETWEEN_CONVERSIONS       (5)
#define ACCUMULATE_SAMPLES_COUNT        (100)
#define MEAS_TIMEOUT                    (100) // ms

#define BATTERY_VOLTAGE_MAX             (4100)
#define BATTERY_VOLTAGE_MIN             (2900)


static const int16_t battery_voltage_offset = 28;
static uint16_t sysmon_battery_voltage = BATTERY_VOLTAGE_MAX; // mV
static uint8_t  sysmon_battery_charge = 99; // %


//  ***************************************************************************
/// @brief  System monitor initialization
/// @param  none
/// @return none
//  ***************************************************************************
void sysmon_init(void) {
    
    // Initialization charger detection pin
    gpio_set_mode        (GPIOA, CHARGER_DETECTION_PIN, GPIO_MODE_INPUT);
    gpio_set_output_speed(GPIOA, CHARGER_DETECTION_PIN, GPIO_SPEED_LOW);
    gpio_set_pull        (GPIOA, CHARGER_DETECTION_PIN, GPIO_PULL_UP);

    adc_init();
}

//  ***************************************************************************
/// @brief  Check charger connect
/// @param  none
/// @return true - charger is connected, false - work from battery
//  ***************************************************************************
bool sysmon_is_charger_connect(void) {
    return GPIOA->IDR & (0x01u << CHARGER_DETECTION_PIN);
}

//  ***************************************************************************
/// @brief  System monitor process
/// @param  none
/// @return none
//  ***************************************************************************
bool sysmon_calc_battery_voltage(void) {
    
    // Gathering raw data
    adc_state_state(true);
    uint32_t acc_adc_bins = 0;
    for (uint32_t i = 0; i < ACCUMULATE_SAMPLES_COUNT; ++i) {
        
        // Start conversion
        adc_start_conversion();
        
        // Wait conversion complete
        uint64_t start_time = get_time_ms();
        while (!adc_is_conversion_complete()) {
            if (get_time_ms() - start_time > MEAS_TIMEOUT) {
                return false;
            }
        }
        
        // Accumulate
        acc_adc_bins += adc_get_conversion_result();
        
        // Pause
        start_time = get_time_ms();
        while (get_time_ms() - start_time < PAUSE_BETWEEN_CONVERSIONS);
    }
    adc_state_state(false);
    
    // Revert voltage divisor factor (voltage_div_factor = 1 / real_factor)
    // Voltage divisor: VIN-[10k]-OUT-[10k]-GND
    // * 1000 - convert V to mV
    const float voltage_div_factor = ((10000.0f + 10000.0f) / 10000.0f) * 1000.0f;
    const float bins_to_voltage_factor = 3.3f / 4096.0f;
    
    // Battery voltage (max voltage 4.2V)
    uint32_t avr_adc_bins = acc_adc_bins / ACCUMULATE_SAMPLES_COUNT;
    float input_voltage = avr_adc_bins * bins_to_voltage_factor;
    int32_t battery_voltage = (uint32_t)(input_voltage * voltage_div_factor);

    // Offset battery voltage
    battery_voltage += battery_voltage_offset;
    if (battery_voltage > BATTERY_VOLTAGE_MAX) {
        battery_voltage = BATTERY_VOLTAGE_MAX;
    }
    if (battery_voltage < BATTERY_VOLTAGE_MIN) {
        battery_voltage = BATTERY_VOLTAGE_MIN;
    }
    if (sysmon_battery_voltage > battery_voltage || sysmon_is_charger_connect()) {
        sysmon_battery_voltage = battery_voltage; // Allow increase voltage value for charge mode
    }
    
    // Calculate battery charge persents
    float battery_charge = (float)(sysmon_battery_voltage - BATTERY_VOLTAGE_MIN) / (float)(BATTERY_VOLTAGE_MAX - BATTERY_VOLTAGE_MIN) * 100.0f;
    sysmon_battery_charge = (uint8_t)battery_charge;
    
    if (sysmon_battery_charge < 1) {
        sysmon_battery_charge = 1;
    }
    if (sysmon_battery_charge > 100) {
        sysmon_battery_charge = 100;
    }
    return true;
}

//  ***************************************************************************
/// @brief  Get battery charge
/// @note   none
/// @return battery charge, % [1; 100]
//  ***************************************************************************
uint8_t sysmon_get_battery_charge(void) {
    return sysmon_battery_charge;
}
