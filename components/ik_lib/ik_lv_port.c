#include "ik_lv_port.h"
#include "lcd_st7789.h"
#include "ik_heap.h"
#include "ik_tim.h"
#include "lvgl.h"
#include "lv_examples.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

// LVGL param
#define BYTE_PER_PIXEL (LV_COLOR_FORMAT_GET_SIZE(LV_COLOR_FORMAT_RGB565)) /*will be 2 for RGB565 */
#define LV_SCREEN_SIZE  ST7789_WIDTH * ST7789_HEIGHT / 10 * BYTE_PER_PIXEL
static lv_display_t *display = NULL;
static uint8_t *lv_screen_buf = NULL;

static uint32_t tick_ms_get(void)
{
    return ik_tim_get_tick_ms();
}

static void my_disp_flush(lv_display_t * disp, const lv_area_t * area, lv_color_t * color_p)
{
    st7789.draw_multi_pixel( area->x1, area->x2, area->y1, area->y2, (uint16_t *)color_p );
    lv_display_flush_ready(disp);         /* Indicate you are ready with the flushing*/
}

static void lvgl_loop( void *arg )
{
    lv_example_get_started_1();

    while(1)
    {
        lv_timer_handler();
        vTaskDelay(10);
    }
    
    return;
}

void ik_lv_init( void )
{
    lv_init();
    lv_tick_set_cb( tick_ms_get );
    display = lv_display_create(ST7789_WIDTH, ST7789_HEIGHT);

    /*Declare a buffer for 1/10 screen size*/
    lv_screen_buf = IK_MALLOC( LV_SCREEN_SIZE );
    if( lv_screen_buf != NULL )
    {
        lv_display_set_buffers(display, lv_screen_buf, NULL, LV_SCREEN_SIZE, LV_DISPLAY_RENDER_MODE_PARTIAL);  /*Initialize the display buffer.*/
        lv_display_set_flush_cb(display, my_disp_flush);
        if( xTaskCreatePinnedToCore( lvgl_loop, "lvgl_loop", 1024*30, NULL, 1, NULL, 0 ) == pdTRUE )
        {
            // ESP_LOGI( "lvgl", "lvgl task start\r\n" );
        }
    }
}
