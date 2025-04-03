#include "ik_iic_hal.h"
#include "driver/i2c_master.h"

#define I2C_MASTER_SCL_IO 18
#define I2C_MASTER_SDA_IO 17

static i2c_master_dev_handle_t dev_handle;
static i2c_master_bus_handle_t bus_handle;

static int ik_iic_master_open( void )
{
    i2c_master_bus_config_t i2c_mst_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = -1,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = 0x6B,
        .scl_speed_hz = 100000,
    };

    i2c_new_master_bus(&i2c_mst_config, &bus_handle);
    i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle);
    return 0;
}

static int ik_iic_master_close( void )
{
    i2c_master_bus_rm_device(dev_handle);
    return 0;
}

static int ik_iic_master_transfer( uint8_t *tx_buf, uint32_t tx_len, uint8_t *rx_buf, uint32_t rx_len )
{
    if( ( rx_buf == NULL ) || ( rx_len == 0 ) )
    {
        i2c_master_transmit( dev_handle, tx_buf, tx_len, 50 );
    }
    else
    {
        i2c_master_transmit_receive( dev_handle, tx_buf, tx_len, rx_buf, rx_len, 50 );
    }
    return 0;
}

IK_IIC_MASTER_HAL_STRUCT ik_iic = {
    .open = ik_iic_master_open,
    .close = ik_iic_master_close,
    .transfer = ik_iic_master_transfer,
};
