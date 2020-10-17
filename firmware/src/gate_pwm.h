//  ***************************************************************************
/// @file    gate_pwm.h
/// @author  NeoProg
/// @brief   Gate PWM driver
//  ***************************************************************************
#ifndef _GATE_PWM_H_
#define _GATE_PWM_H_

#include <stdint.h>


extern void gate_pwm_init(void);
extern void gate_pwm_disable(void);
extern void gate_pwm_enable(void);
extern void gate_pwm_set_duty_cycle(uint16_t duty_cycle);


#endif // _GATE_PWM_H_
