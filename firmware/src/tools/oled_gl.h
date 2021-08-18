//  ***************************************************************************
/// @file    oled_gl.h
/// @author  NeoProg
/// @brief   OLED display graphic library
//  ***************************************************************************
#ifndef _OLED_GL_H_
#define _OLED_GL_H_

#include <stdint.h>
#include <stdbool.h>


typedef enum {
    FONT_ID_6x8,
    FONT_ID_24x32
} font_id_t;


extern bool oled_gl_init(void);
extern void oled_gl_clear_display(void);
extern void oled_gl_draw_point(uint32_t row, uint32_t x, uint32_t y, bool is_show);
extern void oled_gl_draw_string(uint32_t row, uint32_t x, const char* str, font_id_t font_id);
extern void oled_gl_draw_horizontal_line(uint32_t row, uint32_t x, uint32_t y, uint32_t width);
extern bool oled_gl_display_update(void);


#endif /* _OLED_GL_H_ */
