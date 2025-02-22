#include "holocubic_app_main.h"

#define LOG_TAG "APP"

static void chip_info_print( void )
{
    esp_chip_info_t chip_info;
    uint32_t flash_size;

    /* Print chip information */
    esp_chip_info(&chip_info);

    ESP_LOGI( LOG_TAG, "This is %s chip with %d CPU core(s), %s%s%s%s, ",
        CONFIG_IDF_TARGET,
        chip_info.cores,
        (chip_info.features & CHIP_FEATURE_WIFI_BGN) ? "WiFi/" : "",
        (chip_info.features & CHIP_FEATURE_BT) ? "BT" : "",
        (chip_info.features & CHIP_FEATURE_BLE) ? "BLE" : "",
        (chip_info.features & CHIP_FEATURE_IEEE802154) ? ", 802.15.4 (Zigbee/Thread)" : "");

    ESP_LOGI( LOG_TAG, "Minimum free heap size: %" PRIu32 " bytes\n", esp_get_minimum_free_heap_size());
    ESP_LOGI( LOG_TAG, "free heap size: %" PRIu32 " bytes\n", esp_get_free_heap_size());

    esp_flash_get_size(NULL, &flash_size);
    ESP_LOGI( LOG_TAG, " %lu MB %s flash\r\n", flash_size / (uint32_t)(1024 * 1024),
        (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");
}

static void lcd_self_test( void *arg )
{
    st7789.fill_screen(WHITE);
    vTaskDelay(2000);
    while(1)
    {
        // st7789.draw_line( 10, 10, 100, 100, BLACK );
        // vTaskDelay(2000);
        // st7789.draw_line( 10, 10, 100, 100, RED );
        // vTaskDelay(2000);
        // st7789.draw_line( 10, 10, 100, 100, BLUE );
        // vTaskDelay(2000);

        st7789.fill_screen(WHITE);
        vTaskDelay(2000);
        st7789.draw_rectangle( 10, 10, 50, 50, GREEN );
        vTaskDelay(2000);
        st7789.fill_screen(WHITE);
        vTaskDelay(2000);
        st7789.fill_rectangle( 10, 10, 50, 50, GREEN );
        vTaskDelay(2000);

        st7789.fill_screen(WHITE);
        vTaskDelay(2000);
        st7789.draw_circle( 100, 100, 20, YELLOW );
        vTaskDelay(2000);
        st7789.fill_screen(WHITE);
        vTaskDelay(2000);
        st7789.fill_circle( 100, 100, 20, YELLOW );
        vTaskDelay(2000);
    }
}

void holocubic_main( void )
{
    st7789.init();
    chip_info_print();
    xTaskCreatePinnedToCore( lcd_self_test, "lcd", 2048, NULL, 1, NULL, 0 );
    return;
}