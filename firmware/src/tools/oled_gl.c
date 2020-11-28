//  ***************************************************************************
/// @file    oled_gl.c
/// @author  NeoProg
//  ***************************************************************************
#include "oled_gl.h"
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "ssd1306_128x64.h"

#define FONT_SYMBOL_OFFSET      ((uint32_t)' ')
typedef struct {
    uint8_t width;
    uint8_t height;
    const uint8_t* symbols[95];
} font_t;

#include "oled_gl_font_6x8.h"
#include "oled_gl_font_24x32.h"


static font_t font_list[2] = {0};


//  ***************************************************************************
/// @brief  Graphic library initialization
/// @return none
//  ***************************************************************************
bool oled_gl_init(void) {
    
    // Font registration
    font_6x8_init(&font_list[FONT_ID_6x8]);
    font_24x32_init(&font_list[FONT_ID_24x32]);
    
    // Display initialization
    if (!ssd1306_128x64_init())             { return false; }
    if (!ssd1306_128x64_set_inverse(false)) { return false; }
    if (!ssd1306_128x64_set_contrast(0xFF)) { return false; }
    if (!ssd1306_128x64_set_state(true))    { return false; }
    
    return true;
}

//  ***************************************************************************
/// @brief  Clear display
/// @param  none
/// @return none
//  ***************************************************************************
void oled_gl_clear_display(void) {
    for (uint32_t i = 0; i < DISPLAY_HEIGHT / 8; ++i) {
        uint8_t* frame_buffer = ssd1306_128x64_get_frame_buffer(i, 0);
        memset(frame_buffer, 0x00, DISPLAY_WIDTH);
    }
}

//  ***************************************************************************
/// @brief  Draw point
/// @param  row: display row [0; 7]
/// @param  x, y: point position (relative row)
/// @return none
//  ***************************************************************************
void oled_gl_draw_point(uint32_t row, uint32_t x, uint32_t y, bool is_show) {
    uint8_t* frame_buffer = ssd1306_128x64_get_frame_buffer(row, x);
    if (!is_show) {
        frame_buffer[0] &= ~(1 << y);
    } else {
        frame_buffer[0] |= (1 << y);
    }
}

//  ***************************************************************************
/// @brief  Draw string
/// @param  row: display row [0; 7]
/// @param  x: first symbol position
/// @param  str: string for draw
/// @param  font_id: font ID
/// @return none
//  ***************************************************************************
void oled_gl_draw_string(uint32_t row, uint32_t x, const char* str, font_id_t font_id) {
    
    const font_t* font = &font_list[font_id];
    while (*str != '\0') {
    
        const uint8_t* symbol = font->symbols[*str - FONT_SYMBOL_OFFSET];
        for (uint32_t y = 0; y < font->height / 8; ++y) {
            uint8_t* buffer = ssd1306_128x64_get_frame_buffer(y + row, x);
            if (symbol) {
                memcpy(buffer, symbol, font->width);
                symbol += font->width;
            } 
            else {
                memset(buffer, 0x00, font->width);
            }
        }
        
        // Go to next symbol
        ++str;
        x += font->width;
    }
}

//  ***************************************************************************
/// @brief  Draw horizontal line
/// @param  row: display row [0; 7]
/// @param  x, y: line begin position (relative row)
/// @param  width: line width
/// @return none
//  ***************************************************************************
void oled_gl_draw_horizontal_line(uint32_t row, uint32_t x, uint32_t y, uint32_t width) {
    
    uint8_t* frame_buffer = ssd1306_128x64_get_frame_buffer(row, x);
    uint8_t mask = (1 << y);
    
    if (x + width > DISPLAY_WIDTH) {
        width -= DISPLAY_WIDTH - (x + width);
    }
    
    for (uint32_t i = 0; i < width; ++i) {
        frame_buffer[i] |= mask;
    }
}

//  ***************************************************************************
/// @brief  Synchronous display update
/// @param  none
/// @return none
//  ***************************************************************************
bool oled_gl_display_update(void) {
    return ssd1306_128x64_update();
}
