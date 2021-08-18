//  ***************************************************************************
/// @file    led.h
/// @author  NeoProg
/// @brief   LED control
//  ***************************************************************************
#ifndef _LED_H_
#define _LED_H_


typedef enum {
    STATE_ALARM,
    STATE_WARNING,
    STATE_NORMAL
} led_state_t;


extern void led_init(void);
extern void led_set_state(led_state_t state);
extern void led_process(void);


#endif /* _LED_H_ */
