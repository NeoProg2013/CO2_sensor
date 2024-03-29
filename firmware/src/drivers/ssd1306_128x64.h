//  ***************************************************************************
/// @file    ssd1306_128x64.h
/// @author  NeoProg
/// @brief   SSD1306 128x64 display driver
//  ***************************************************************************
#ifndef _SSD1306_128X64_H_
#define _SSD1306_128X64_H_

#include <stdint.h>
#include <stdbool.h>

#define DISPLAY_WIDTH                   (128)
#define DISPLAY_HEIGHT                  (64)
#define DISPLAY_MAX_ROW_COUNT           (DISPLAY_HEIGHT / 8)


extern bool ssd1306_128x64_init(void);
extern bool ssd1306_128x64_set_contrast(uint8_t contrast);
extern bool ssd1306_128x64_set_inverse(bool is_inverse);
extern bool ssd1306_128x64_set_state(bool is_enable);
extern bool ssd1306_128x64_update(void);
extern uint8_t* ssd1306_128x64_get_frame_buffer(uint32_t row, uint32_t column);


#endif // _SSD1306_128X64_H_
