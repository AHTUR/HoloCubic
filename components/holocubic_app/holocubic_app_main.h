#ifndef _HOLOCUBIC_APP_MAIN_H
#define _HOLOCUBIC_APP_MAIN_H

// Freertos
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// ESP32 platform
#include "sdkconfig.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"

// IK_BSP
#include "ik_spi_hal.h"

// IK_LIB
#include "ik_heap.h"
#include "ik_tim.h"
#include "ik_lv_port.h"

// LCD
#include "lcd_st7789.h"

void holocubic_main( void );

#endif
