//  ***************************************************************************
/// @file    display.h
/// @author  NeoProg
/// @brief   Display control
//  ***************************************************************************
#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include <stdint.h>
#include <stdbool.h>

#define CONC_UNKNOWN_VALUE        (0xFFFF)
#define CHARGE_UNKNOWN_VALUE      (0xFF)


extern bool display_init(void);
extern bool display_update(uint16_t conc, uint8_t battery_charge);


#endif /* _DISPLAY_H_ */
