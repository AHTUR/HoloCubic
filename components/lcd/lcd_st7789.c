// ESP
#include "lcd_st7789.h"
#include "ik_spi_hal.h"
#include "driver/gpio.h"
#include "esp_log.h"

// Freertos
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define LCD_ST7789_DC_PIN       40
#define LCD_ST7789_BCK_PIN      46
#define LCD_ST7789_RES_PIN      45

#define ST7789_WIDTH  240  // 屏幕宽度
#define ST7789_HEIGHT 240  // 屏幕高度

// ST7789 命令定义
#define ST7789_RESET 0X01
#define ST7789_CASET 0x2A  // 列地址设置
#define ST7789_RASET 0x2B  // 行地址设置
#define ST7789_RAMWR 0x2C  // 内存写入

static uint8_t *frame_buf = NULL;

static void lcd_st7789_delay_ms( uint32_t ms )
{
    vTaskDelay(ms);
    return;
}

static void lcd_st7789_dc_set( void )
{
    gpio_set_level( LCD_ST7789_DC_PIN, 1 );
    return;
}

static void lcd_st7789_dc_clean( void )
{
    gpio_set_level( LCD_ST7789_DC_PIN, 0 );
    return;
}

static void lcd_st7789_bck_close( void )
{
    gpio_set_level( LCD_ST7789_BCK_PIN, 1 );
    return;
}

static void lcd_st7789_bck_open( void )
{
    gpio_set_level( LCD_ST7789_BCK_PIN, 0 );
    return;
}

static void lcd_st7789_reset( void )
{
    gpio_set_level( LCD_ST7789_RES_PIN, 0 );
    lcd_st7789_delay_ms(1);
    gpio_set_level( LCD_ST7789_RES_PIN, 1 );
    lcd_st7789_delay_ms(150);
    return;
}

static int lcd_st7789_write_cmd( uint8_t cmd )
{
    uint8_t tx_buf[10] = {0};
    lcd_st7789_dc_clean();
    tx_buf[0] = cmd;
    return ik_spi0.transfer( tx_buf, 1, NULL, 0 );
}

static int lcd_st7789_write_data( uint8_t *data, uint32_t size )
{
	int ret = 0;
	uint32_t total_size = 0, pos = 0;

    lcd_st7789_dc_set();

	total_size = size;
	pos = 0;
	while ( total_size > 0 )
	{
		if( total_size >= ik_spi0.max_transfer_size )
		{
			ret = ik_spi0.transfer( data + pos, ik_spi0.max_transfer_size, NULL, 0 );
			if( ret != 0 )
			{
				break;
			}
			pos += ik_spi0.max_transfer_size;
			total_size -= ik_spi0.max_transfer_size;
		}
		else
		{
			ret = ik_spi0.transfer( data + pos, total_size, NULL, 0 );
			if( ret != 0 )
			{
				break;
			}
			total_size = 0;
		}
	}

    return ret;
}

static int lcd_st7789_write_one_data( uint8_t data )
{
    uint8_t tx_buf[10] = {0};
	tx_buf[0] = data;
	return lcd_st7789_write_data( tx_buf, 1 );
}

static void lcd_st7789_set_window(uint16_t x_start, uint16_t x_end, uint16_t y_start, uint16_t y_end) 
{
	uint8_t data[4] = {0};

    // 设置列地址
    lcd_st7789_write_cmd(ST7789_CASET);
	data[0] = ( x_start >> 8 ) & 0xFF;
	data[1] = x_start & 0xFF;
	data[2] = ( x_end >> 8 ) & 0xFF;
	data[3] = x_end & 0xFF;
	// lcd_st7789_write_one_data(data[0]);
	// lcd_st7789_write_one_data(data[1]);
	// lcd_st7789_write_one_data(data[2]);
	// lcd_st7789_write_one_data(data[3]);
	lcd_st7789_write_data( data, 4 );

    // 设置行地址
	lcd_st7789_write_cmd(ST7789_RASET);
	data[0] = ( y_start >> 8 ) & 0xFF;
	data[1] = y_start & 0xFF;
	data[2] = ( y_end >> 8 ) & 0xFF;
	data[3] = y_end & 0xFF;
	// lcd_st7789_write_one_data(data[0]);
	// lcd_st7789_write_one_data(data[1]);
	// lcd_st7789_write_one_data(data[2]);
	// lcd_st7789_write_one_data(data[3]);
	lcd_st7789_write_data( data, 4 );
}

/**
 * @brief 刷新屏幕为指定颜色
 * @param spi SPI 设备句柄
 * @param color 16 位 RGB565 颜色值
 */
static void lcd_st7789_fill_screen(uint16_t color) 
{
	uint32_t y = 0;
    // 设置全屏显示区域
    lcd_st7789_set_window(0, ST7789_WIDTH - 1, 0, ST7789_HEIGHT - 1);

	for( y = 0; y < ( ST7789_HEIGHT * ST7789_WIDTH ); y++ )
	{
		frame_buf[y*2] = (color >> 8) & 0xFF;
		frame_buf[y*2+1] = color & 0xFF;
	}

	// 启动内存写入
	lcd_st7789_write_cmd(ST7789_RAMWR);

    // 填充屏幕
	lcd_st7789_write_data(frame_buf, ( ST7789_HEIGHT * ST7789_WIDTH * 2 ) );

	return;
}

static int lcd_st7789_cfg_0( void )
{
    lcd_st7789_bck_open();
    lcd_st7789_delay_ms(100);

	lcd_st7789_write_cmd(0x11);	//Sleep Out
	lcd_st7789_delay_ms(255);
	
	lcd_st7789_write_cmd(0x3A);	//Interface Pixel Format
	lcd_st7789_write_one_data(0x55);
	lcd_st7789_delay_ms(10);
	
	lcd_st7789_write_cmd(0x36);	//Memory Data Access Control
	lcd_st7789_write_one_data(0x00);

	lcd_st7789_write_cmd(0x2A);	//Column Address Set
	lcd_st7789_write_one_data(0x00);
	lcd_st7789_write_one_data(0x00);
	lcd_st7789_write_one_data(0x00);
	lcd_st7789_write_one_data(0xF0);

	lcd_st7789_write_cmd(0x2B);	//Row Address Set
	lcd_st7789_write_one_data(0x00);
	lcd_st7789_write_one_data(0x00);
	lcd_st7789_write_one_data(0x00);
	lcd_st7789_write_one_data(0xF0);

	lcd_st7789_write_cmd(0x21);	//Display Inversion On
	lcd_st7789_delay_ms(10);

	lcd_st7789_write_cmd(0x13);	//Normal Display Mode On
	lcd_st7789_delay_ms(10);

	lcd_st7789_write_cmd(0x29);	//Display ON
	lcd_st7789_delay_ms(10);
	return 0;
}

static int lcd_st7789_cfg_1( void )
{
    lcd_st7789_bck_open();
    lcd_st7789_delay_ms(100);
	
	lcd_st7789_write_cmd(0x11); //Sleep out 
	lcd_st7789_delay_ms(255);     

	lcd_st7789_write_cmd(0x36);
	lcd_st7789_write_one_data(0x00);

	lcd_st7789_write_cmd(0x3A); 
	lcd_st7789_write_one_data(0x55);

	lcd_st7789_write_cmd(0xB2);
	lcd_st7789_write_one_data(0x0C);
	lcd_st7789_write_one_data(0x0C);
	lcd_st7789_write_one_data(0x00);
	lcd_st7789_write_one_data(0x33);
	lcd_st7789_write_one_data(0x33); 

	lcd_st7789_write_cmd(0xB7); 
	lcd_st7789_write_one_data(0x35);  

	lcd_st7789_write_cmd(0xBB);
	lcd_st7789_write_one_data(0x19);

	lcd_st7789_write_cmd(0xC0);
	lcd_st7789_write_one_data(0x2C);

	lcd_st7789_write_cmd(0xC2);
	lcd_st7789_write_one_data(0x01);

	lcd_st7789_write_cmd(0xC3);
	lcd_st7789_write_one_data(0x12);   

	lcd_st7789_write_cmd(0xC4);
	lcd_st7789_write_one_data(0x20);  

	lcd_st7789_write_cmd(0xC6); 
	lcd_st7789_write_one_data(0x0F);    

	lcd_st7789_write_cmd(0xD0); 
	lcd_st7789_write_one_data(0xA4);
	lcd_st7789_write_one_data(0xA1);

	lcd_st7789_write_cmd(0xE0);
	lcd_st7789_write_one_data(0xD0);
	lcd_st7789_write_one_data(0x04);
	lcd_st7789_write_one_data(0x0D);
	lcd_st7789_write_one_data(0x11);
	lcd_st7789_write_one_data(0x13);
	lcd_st7789_write_one_data(0x2B);
	lcd_st7789_write_one_data(0x3F);
	lcd_st7789_write_one_data(0x54);
	lcd_st7789_write_one_data(0x4C);
	lcd_st7789_write_one_data(0x18);
	lcd_st7789_write_one_data(0x0D);
	lcd_st7789_write_one_data(0x0B);
	lcd_st7789_write_one_data(0x1F);
	lcd_st7789_write_one_data(0x23);

	lcd_st7789_write_cmd(0xE1);
	lcd_st7789_write_one_data(0xD0);
	lcd_st7789_write_one_data(0x04);
	lcd_st7789_write_one_data(0x0C);
	lcd_st7789_write_one_data(0x11);
	lcd_st7789_write_one_data(0x13);
	lcd_st7789_write_one_data(0x2C);
	lcd_st7789_write_one_data(0x3F);
	lcd_st7789_write_one_data(0x44);
	lcd_st7789_write_one_data(0x51);
	lcd_st7789_write_one_data(0x2F);
	lcd_st7789_write_one_data(0x1F);
	lcd_st7789_write_one_data(0x1F);
	lcd_st7789_write_one_data(0x20);
	lcd_st7789_write_one_data(0x23);
	lcd_st7789_write_cmd(0x21); 
	lcd_st7789_write_cmd(0x29); 
	lcd_st7789_delay_ms(10);
	return 0;
}

static int lcd_st7789_init( void )
{
	int ret = 0;
    gpio_config_t io_conf = {0};
    io_conf.pin_bit_mask = ((1ULL << LCD_ST7789_DC_PIN) | (1ULL << LCD_ST7789_RES_PIN) | (1ULL << LCD_ST7789_BCK_PIN));
    io_conf.mode = GPIO_MODE_OUTPUT;
    gpio_config(&io_conf);

	ret = ik_spi0.open();
	if( ret != 0 )
	{
		ESP_LOGI( "lcd", "spi open fail\r\n" );
		return -1;
	}

	frame_buf = malloc( ST7789_HEIGHT * ST7789_WIDTH * 2 );
	if( frame_buf == NULL )
	{
		ESP_LOGI( "lcd", "lcd frame buf malloc fail\r\n" );
		return -2;
	}

    st7789.reset();

	// lcd_st7789_write_cmd(0x01);	//sw reset
	// lcd_st7789_delay_ms(100);

	lcd_st7789_cfg_1();

    return 0;
}

static int lcd_st7789_deinit( void )
{
    lcd_st7789_reset();
	free( frame_buf );
	frame_buf = NULL;
	ik_spi0.close();
	return 0;
}

static int lcd_st7789_draw_pixel( uint16_t x, uint16_t y, uint16_t color )
{
	uint8_t data[2] = {0};

	if( ( x > ST7789_WIDTH ) || ( y > ST7789_HEIGHT ) )
	{
		return -1;
	}

	lcd_st7789_set_window( x, x, y, y );
	// 启动内存写入
	lcd_st7789_write_cmd(ST7789_RAMWR);
	data[0] = ( color >> 8 ) & 0xff;
	data[1] = color & 0xff;
    // 填充像素
	lcd_st7789_write_data( data, 2 );
	return 0;
}

static int lcd_st7789_draw_line( uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color )
{
	int i;
	int dx,dy;
	int sx,sy;
	int E;

	if( ( x0 > ST7789_WIDTH ) || ( y0 > ST7789_HEIGHT ) || ( x1 > ST7789_WIDTH ) || ( y1 > ST7789_HEIGHT ) )
	{
		return -1;
	}

	/* distance between two points */
	dx = ( x1 > x0 ) ? x1 - x0 : x0 - x1;
	dy = ( y1 > y0 ) ? y1 - y0 : y0 - y1;

	/* direction of two point */
	sx = ( x1 > x0 ) ? 1 : -1;
	sy = ( y1 > y0 ) ? 1 : -1;

	if ( dx > dy ) 	/* inclination < 1 */
	{
		// 斜率<1的情况
		E = -dx;
		for ( i = 0 ; i <= dx ; i++ ) 
		{
			lcd_st7789_draw_pixel( x0, y0, color);
			x0 += sx;
			E += 2 * dy;
			if ( E >= 0 ) 
			{
				y0 += sy;
				E -= 2 * dx;
			}
		}
	} 
	else /* inclination >= 1 */
	{
		// 斜率 > 1的情况，将斜率<1的情况x,y坐标对调即可变换
		E = -dy;
		for ( i = 0 ; i <= dy ; i++ ) 
		{
			lcd_st7789_draw_pixel( x0, y0, color);
			y0 += sy;
			E += 2 * dx;
			if ( E >= 0 ) 
			{
				x0 += sx;
				E -= 2 * dy;
			}
		}
	}

	return 0;
}

static int lcd_st7789_draw_rectangle( uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color )
{
	if( ( x0 >= ST7789_WIDTH ) || ( x1 >= ST7789_WIDTH ) || ( y0 >= ST7789_HEIGHT ) || ( y1 >= ST7789_HEIGHT ) ||
		( x0 > x1 ) || ( y0 > y1 ) )
	{
		return -1;
	}

	lcd_st7789_draw_line( x0, y0, x1, y0, color );
	// ESP_LOGI( "lcd", "rect %u %u %u %u\r\n", x0, y0, x1, y0 );
	lcd_st7789_draw_line( x1, y0, x1, y1, color );
	// ESP_LOGI( "lcd", "rect %u %u %u %u\r\n", x1, y0, x1, y1 );
	lcd_st7789_draw_line( x1, y1, x0, y1, color );
	// ESP_LOGI( "lcd", "rect %u %u %u %u\r\n", x1, y1, x0, y1 );
	lcd_st7789_draw_line( x0, y1, x0, y0, color );
	// ESP_LOGI( "lcd", "rect %u %u %u %u\r\n", x0, y1, x0, y0 );
	return 0;
}

static int lcd_st7789_fill_rectangle( uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color )
{
	uint16_t y_pos = 0;

	if( ( x0 >= ST7789_WIDTH ) || ( x1 >= ST7789_WIDTH ) || ( y0 >= ST7789_HEIGHT ) || ( y1 >= ST7789_HEIGHT ) ||
		( x0 > x1 ) || ( y0 > y1 ) )
	{
		return -1;
	}

	for( y_pos = y0; y_pos <= y1; y_pos++ )
	{
		st7789.draw_line( x0, y_pos, x1, y_pos, color );
	}

	return 0;
}

static int lcd_st7789_draw_circle( uint16_t x0, uint16_t y0, uint16_t r, uint16_t color )
{
	int x = 0;
    int y = r;
    int d = 1 - r;

	if( ( x0 >= ST7789_WIDTH ) || ( y0 >= ST7789_HEIGHT ) )
	{
		return -1;
	}

    while (x <= y) {
        // 绘制八个对称点
        lcd_st7789_draw_pixel(x0 + x, y0 + y, color);
        lcd_st7789_draw_pixel(x0 + y, y0 + x, color);
        lcd_st7789_draw_pixel(x0 - x, y0 + y, color);
        lcd_st7789_draw_pixel(x0 - y, y0 + x, color);
        lcd_st7789_draw_pixel(x0 + x, y0 - y, color);
        lcd_st7789_draw_pixel(x0 + y, y0 - x, color);
        lcd_st7789_draw_pixel(x0 - x, y0 - y, color);
        lcd_st7789_draw_pixel(x0 - y, y0 - x, color);

        if (d < 0) {
            d += 2 * x + 3;
        } else {
            d += 2 * (x - y) + 5;
            y--;
        }
        x++;
    }

	return 0;
}

static int lcd_st7789_fill_circle( uint16_t x0, uint16_t y0, uint16_t r, uint16_t color )
{
    int x = 0;
    int y = r;
    int d = 1 - r;

	if( ( x0 >= ST7789_WIDTH ) || ( y0 >= ST7789_HEIGHT ) )
	{
		return -1;
	}

    while (x <= y) {
        // 绘制水平线填充圆内的区域
        lcd_st7789_draw_line(x0 - x, y0 + y, x0 + x, y0 + y, color);
        lcd_st7789_draw_line(x0 - x, y0 - y, x0 + x, y0 - y, color);
        lcd_st7789_draw_line(x0 - y, y0 + x, x0 + y, y0 + x, color);
        lcd_st7789_draw_line(x0 - y, y0 - x, x0 + y, y0 - x, color);

        if (d < 0) {
            d += 2 * x + 3;
        } else {
            d += 2 * (x - y) + 5;
            y--;
        }
        x++;
    }

	return 0;
}

LCD_ST7789_STRUCT st7789 = {
    .write_cmd = lcd_st7789_write_cmd,
    .write_data = lcd_st7789_write_data,
    .init = lcd_st7789_init,
    .deinit = lcd_st7789_deinit,
    .reset = lcd_st7789_reset,
	.fill_screen = lcd_st7789_fill_screen,
	.draw_line = lcd_st7789_draw_line,
	.draw_rectangle = lcd_st7789_draw_rectangle,
	.fill_rectangle = lcd_st7789_fill_rectangle,
	.draw_circle = lcd_st7789_draw_circle,
	.fill_circle = lcd_st7789_fill_circle,
};
