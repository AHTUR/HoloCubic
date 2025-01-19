#ifndef _LCD_ST7789_H
#define _LCD_ST7789_H

#include "stdint.h"

#define rgb565(r, g, b) (((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3))

#define RED    rgb565(255,   0,   0) // 0xf800
#define GREEN  rgb565(  0, 255,   0) // 0x07e0
#define BLUE   rgb565(  0,   0, 255) // 0x001f
#define BLACK  rgb565(  0,   0,   0) // 0x0000
#define WHITE  rgb565(255, 255, 255) // 0xffff
#define GRAY   rgb565(128, 128, 128) // 0x8410
#define YELLOW rgb565(255, 255,   0) // 0xFFE0
#define CYAN   rgb565(  0, 156, 209) // 0x04FA
#define PURPLE rgb565(128,   0, 128) // 0x8010

typedef struct 
{
    int (*write_cmd)( uint8_t cmd );
    int (*write_data)( uint8_t *data, uint32_t size );
    int (*init)( void );
    int (*deinit)( void );
    void (*reset)( void );
    void (*fill_screen)( uint16_t color );
} LCD_ST7789_STRUCT;

extern LCD_ST7789_STRUCT st7789;

#endif
