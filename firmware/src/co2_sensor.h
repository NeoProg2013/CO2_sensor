//  ***************************************************************************
/// @file    co2_sensor.h
/// @author  NeoProg
/// @brief   Measurements loop
//  ***************************************************************************
#ifndef _CO2_SENSOR_H_
#define _CO2_SENSOR_H_

#include <stdint.h>


extern void co2_sensor_init(void);
extern bool co2_sensor_read_concentration(void);
extern uint16_t co2_sensor_get_concentration(void);


#endif /* _CO2_SENSOR_H_ */
