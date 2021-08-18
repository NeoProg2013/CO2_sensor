//  ***************************************************************************
/// @file    main.c
/// @author  NeoProg
//  ***************************************************************************
#include "project_base.h"
#include "co2_sensor.h"
#include "systimer.h"
#include "usart1.h"



static uint16_t concentration = 0;
static bool is_data_ready = false;


static void frame_received_callback(uint32_t frame_size);
static void dummy_callback(void);

/*
char getCheckSum(const uint8_t *packet)
{
char i, checksum;
for( i = 1; i < 8; i++)
{
checksum += packet[i];
}
checksum = 0xff - checksum;
checksum += 1;
 return checksum;
}*/


//  ***************************************************************************
/// @brief  Initialize CO2 sensor driver
/// @param  none
/// @return none
//  ***************************************************************************
void co2_sensor_init(void) {
    usart1_callbacks_t callback;
    callback.frame_received_callback = frame_received_callback;
    callback.frame_transmitted_callback = dummy_callback;
    callback.frame_error_callback = dummy_callback;
    usart1_init(9600, &callback);
    
    // Disable ABC
    const uint8_t disable_abc_cmd[9] = {0xFF, 0x01, 0x79, 0x00, 0x00, 0x00, 0x00, 0x00, 0x86}; 
    uint8_t* tx_buffer = usart1_get_tx_buffer();
    memcpy(tx_buffer, disable_abc_cmd, sizeof(disable_abc_cmd));
    usart1_start_tx(sizeof(disable_abc_cmd));
    
    // Delay for send command
    uint64_t start_time = get_time_ms();
    while (get_time_ms() - start_time < 1000);
}

//  ***************************************************************************
/// @brief  Read concentration from CO2 sensor
/// @param  none
/// @return true - success, false - error
//  ***************************************************************************
bool co2_sensor_read_concentration(void) {
    
    const uint8_t meas_cmd[9] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79}; 
    uint8_t* tx_buffer = usart1_get_tx_buffer();
    uint8_t* rx_buffer = usart1_get_rx_buffer();
    
    is_data_ready = false;
    
    // Send measurement command
    memcpy(tx_buffer, meas_cmd, sizeof(meas_cmd));
    usart1_start_rx();
    usart1_start_tx(sizeof(meas_cmd));
    
    // Wait response
    uint64_t start_time = get_time_ms();
    while (!is_data_ready) {
        if (get_time_ms() - start_time > 5000) {
            return false;
        }
    }
    
    // Process response
    concentration = (uint16_t)(rx_buffer[2] << 8) | (rx_buffer[3] << 0);
    if (concentration > 9999) { 
        concentration = 9999;
    }
    return true;
}

//  ***************************************************************************
/// @brief  Get concentration value
/// @param  none
/// @return concentration valuenone
//  ***************************************************************************
uint16_t co2_sensor_get_concentration(void) {
    return concentration;
}





//  ***************************************************************************
/// @brief  USART frame received callback
/// @param  frame_size: received frame size
/// @return none
//  ***************************************************************************
static void frame_received_callback(uint32_t frame_size) {
    is_data_ready = true;
}

//  ***************************************************************************
/// @brief  Dummy
//  ***************************************************************************
static void dummy_callback(void) {
    return;
}
