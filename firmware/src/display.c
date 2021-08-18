//  ***************************************************************************
/// @file    diplay.c
/// @author  NeoProg
//  ***************************************************************************
#include "project_base.h"
#include "display.h"
#include "oled_gl.h"
#include "ssd1306_128x64.h"
#include "systimer.h"
#define CONC_UNKNOWN_VALUE_STR          "----"
#define CHARGE_UNKNOWN_VALUE_STR        "---%"



//  ***************************************************************************
/// @brief  Display init
/// @param  none
/// @return true - success, false - error
//  ***************************************************************************
bool display_init(void) {
    if (!oled_gl_init()) {
        return false;
    }
    oled_gl_draw_string(0, 0, "CO2 sensor", FONT_ID_6x8);
    oled_gl_draw_string(0, 99, CHARGE_UNKNOWN_VALUE_STR, FONT_ID_6x8);
    oled_gl_draw_horizontal_line(1, 0, 7, 128);
    oled_gl_draw_string(3, 8, CONC_UNKNOWN_VALUE_STR, FONT_ID_24x32);
    oled_gl_draw_string(6, 105, "PPM", FONT_ID_6x8);
    return display_update(CONC_UNKNOWN_VALUE, CHARGE_UNKNOWN_VALUE);
}

//  ***************************************************************************
/// @brief  Update display
/// @param  conc: concenctration value
/// @param  battery_charge: battery charge value
/// @return true - success, false - error
//  ***************************************************************************
bool display_update(uint16_t conc, uint8_t battery_charge) {
    
    static bool active_flag = true;
    oled_gl_draw_point(7, 0, 7, active_flag);
    active_flag = !active_flag;
    
    // Convert concentration
    char conc_str[5] = {0};
    if (conc == CONC_UNKNOWN_VALUE) {
        memcpy(conc_str, CONC_UNKNOWN_VALUE_STR, sizeof(conc_str));
    } else {
        if (conc > 9999) {
            conc = 9999;
        }
        sprintf(conc_str, "%4d", conc);
    }
    
    // Convert battery charge
    char charge_str[5] = {0};
    if (battery_charge == CHARGE_UNKNOWN_VALUE) {
        memcpy(charge_str, CHARGE_UNKNOWN_VALUE_STR, sizeof(charge_str));
    } else {
        sprintf(charge_str, "%3d", battery_charge);
    }
    
    // Update display
    oled_gl_draw_string(0, 99, charge_str, FONT_ID_6x8);
    oled_gl_draw_string(3, 8, conc_str, FONT_ID_24x32);
    if (!oled_gl_display_update()) {
        return false;
    }
    return true;
}
