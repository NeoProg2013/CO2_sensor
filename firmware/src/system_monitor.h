//  ***************************************************************************
/// @file    system_monitor.c
/// @author  NeoProg
/// @brief   System monitor interface
//  ***************************************************************************
#ifndef _SYSTEM_MONITOR_H_
#define _SYSTEM_MONITOR_H_

#include <stdint.h>
#include <stdbool.h>


extern void sysmon_init(void);
extern bool sysmon_is_charger_connect(void);
extern bool sysmon_calc_battery_voltage(void);
extern uint8_t sysmon_get_battery_charge(void);


#endif // _SYSTEM_MONITOR_H_
