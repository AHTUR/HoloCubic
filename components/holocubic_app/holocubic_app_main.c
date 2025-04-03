#include "holocubic_app_main.h"

#define LOG_TAG "APP"

static void imu_loop( void *arg )
{
    // uint64_t t_ms = 0;
    float acc[3] = {0};
    float gyro[3] = {0};
    IK_MAHONY_FILTER filter;

    mahony_filter_init( &filter, 2.0f * 0.5f, 2.0f * 0.0f, 100.0f );

    // t_ms = ik_tim_get_tick_ms();
    while(1)
    {
        qmi8658_read_xyz( acc, gyro );

        mahony_filter_input( &filter, gyro[0], gyro[1], gyro[2], acc[0], acc[1], acc[2] );
        mahony_filter_update( &filter );
        mahony_filter_output( &filter );

        vTaskDelay(10);

        // if( ( ik_tim_get_tick_ms() - t_ms ) >= 500 )
        // {
        //     t_ms = ik_tim_get_tick_ms();
        //     ESP_LOGI( LOG_TAG, "acc xyz:%.2f %.2f %.2f gyro xyz: %.2f %.2f %.2f\r\n",
        //         acc[0], acc[1], acc[2], gyro[0], gyro[1], gyro[2] );
        //     ESP_LOGI( LOG_TAG, "1 roll: %.2f pitch: %.2f yaw: %.2f \r\n", angles.roll, angles.pitch, angles.yaw );
        //     ESP_LOGI( LOG_TAG, "2 roll: %.2f pitch: %.2f yaw: %.2f \r\n", filter.roll, filter.pitch, filter.yaw );
        // }
    }
    return;
}

void holocubic_main( void )
{
    ik_tim_init();
    st7789.init();
    ik_lv_init();
    ik_iic.open();

    vTaskDelay(100);
    if( qmi8658_init() == 1 )
    {
        xTaskCreatePinnedToCore( imu_loop, "imu_loop", 1024*8, NULL, 1, NULL, 1 );
    }
    return;
}