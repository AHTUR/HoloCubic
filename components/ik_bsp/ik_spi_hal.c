#include "ik_spi_hal.h"
#include "driver/spi_master.h" 

static spi_device_handle_t spi2_dev;

static int spi_open( void )
{
    esp_err_t ret;
    spi_bus_config_t buscfg = {
        .miso_io_num = -1,
        .mosi_io_num = 42,
        .sclk_io_num = 41,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4092,
    };
    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 50 * 1000 * 1000,     //Clock out at 40 MHz
        .mode = 3,                              //SPI mode 0
        .spics_io_num = -1,                     //CS pin
        .queue_size = 7,                        //We want to be able to queue 7 transactions at a time
        .pre_cb = NULL,                         //Specify pre-transfer callback to handle D/C line
        .address_bits = 0,
        .command_bits = 0,
        .flags = SPI_DEVICE_NO_DUMMY,
    };
    
    //Initialize the SPI bus
    ret = spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO);
    if( ret != ESP_OK )
    {
        return -1;
    }

    //Attach the LCD to the SPI bus
    ret = spi_bus_add_device(SPI2_HOST, &devcfg, &spi2_dev);
    if( ret != ESP_OK )
    {
        return -2;
    }

    ik_spi0.max_transfer_size = 4092;
    return ret;
}

static int spi_close( void )
{
    spi_bus_remove_device( spi2_dev );
    spi_bus_free( SPI2_HOST );
    return 0;
}

static int spi_transfer( uint8_t *tx_buf, uint32_t tx_len, uint8_t *rx_buf, uint32_t rx_len  )
{
    spi_transaction_t data = {0};
    data.tx_buffer = tx_buf;
    data.rx_buffer = rx_buf;
    data.length = tx_len * 8;
    data.rxlength = rx_len * 8;
    return spi_device_transmit( spi2_dev, &data );
    // return spi_device_polling_transmit( spi2_dev, &data );
}

IK_SPI_HAL_STRUCT ik_spi0 = {
    .open = spi_open,
    .close = spi_close,
    .transfer = spi_transfer,
};
