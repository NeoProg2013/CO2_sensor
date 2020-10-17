//  ***************************************************************************
/// @file    main.c
/// @author  NeoProg
//  ***************************************************************************
#include "project_base.h"
#include "systimer.h"
#include "usart1.h"


static void system_init(void);

uint32_t concentration = 0;
bool is_data_ready = false;


void frame_received_callback(uint32_t frame_size) {
    is_data_ready = true;
    return;
}

void frame_transmitted_callback(void) {
    return;
}

void frame_error_callback(void) {
    return;
}



//  ***************************************************************************
/// @brief  Program entry point
/// @param  none
/// @return none
//  ***************************************************************************
int main() {
    
    system_init();
    systimer_init();
    
    usart1_callbacks_t callback;
    callback.frame_received_callback = frame_received_callback;
    callback.frame_transmitted_callback = frame_transmitted_callback;
    callback.frame_error_callback = frame_error_callback;
    usart1_init(9600, &callback);
    
    uint8_t cmd[9] = {0xFF,0x01,0x86,0x00,0x00,0x00,0x00,0x00,0x79}; 
    uint8_t* msg = usart1_get_tx_buffer();
    memcpy(msg, cmd, sizeof(cmd));
    usart1_start_rx();
    usart1_start_tx(sizeof(cmd));
    
    uint64_t last_time = get_time_ms();
    
    while (true) {
        
        if (is_data_ready) {
            
            uint8_t* response = usart1_get_rx_buffer();
            
            concentration = (uint32_t)(response[2] << 8) | (response[3] << 0);
            is_data_ready = false;
        }
        
        if (get_time_ms() - last_time > 1000) {
            usart1_start_rx();
            usart1_start_tx(sizeof(cmd));
            last_time = get_time_ms();
        }
        
        // Check HSE failure
        /*if (RCC->CIR & RCC_CIR_CSSF) {
            gate_pwm_disable();
        }*/
        
        asm("NOP");
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
    
    // Enable clocks for USART1
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
    while ((RCC->APB2ENR & RCC_APB2ENR_USART1EN) == 0);
}