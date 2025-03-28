#include "holocubic_app_main.h"

#define LOG_TAG "APP"

// static void lcd_self_test( void *arg )
// {
//     st7789.fill_screen(WHITE);
//     vTaskDelay(2000);
//     while(1)
//     {
//         // st7789.draw_line( 10, 10, 100, 100, BLACK );
//         // vTaskDelay(2000);
//         // st7789.draw_line( 10, 10, 100, 100, RED );
//         // vTaskDelay(2000);
//         // st7789.draw_line( 10, 10, 100, 100, BLUE );
//         // vTaskDelay(2000);

//         st7789.fill_screen(WHITE);
//         vTaskDelay(2000);
//         st7789.draw_rectangle( 10, 10, 50, 50, GREEN );
//         vTaskDelay(2000);
//         st7789.fill_screen(WHITE);
//         vTaskDelay(2000);
//         st7789.fill_rectangle( 10, 10, 50, 50, GREEN );
//         vTaskDelay(2000);

//         st7789.fill_screen(WHITE);
//         vTaskDelay(2000);
//         st7789.draw_circle( 100, 100, 20, YELLOW );
//         vTaskDelay(2000);
//         st7789.fill_screen(WHITE);
//         vTaskDelay(2000);
//         st7789.fill_circle( 100, 100, 20, YELLOW );
//         vTaskDelay(2000);
//     }
// }

void holocubic_main( void )
{
    ik_tim_init();
    st7789.init();
    ik_lv_init();
    return;
}