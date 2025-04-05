#include "ik_fs_port.h"
#include "stdio.h"
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "driver/sdmmc_host.h"
#include "esp_log.h"

static sdmmc_card_t *card;
static const char mount_point[] = "/sdcard";

int ik_fs_mount( void )
{
    esp_err_t ret;
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024
    };
    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();

    host.max_freq_khz = 5000;
    slot_config.width = 1;
    slot_config.clk = 2;
    slot_config.cmd = 38;
    slot_config.d0 = 1;
    slot_config.flags |= SDMMC_SLOT_FLAG_INTERNAL_PULLUP;
    ret = esp_vfs_fat_sdmmc_mount(mount_point, &host, &slot_config, &mount_config, &card);
    if( ret == ESP_OK )
    {
        ESP_LOGI( "fs", "Mounting filesystem success\r\n");
        sdmmc_card_print_info(stdout, card);
        return 0;
    }
    else
    {
        ESP_LOGI( "fs", "Mounting filesystem fail, %u\r\n", ret );
        return -1;
    } 
}

int ik_fs_unmount( void )
{
    return esp_vfs_fat_sdcard_unmount(mount_point, card);
}

int ik_fs_format( void )
{
    return esp_vfs_fat_sdcard_format(mount_point, card);
}

int ik_fs_open( char *fname, IK_FILE_HANDLE *f_handle )
{
    f_handle->f_p = fopen( fname, "r+b" );
    if( f_handle->f_p == NULL )
    {
        f_handle->f_p = fopen( fname, "wb" );
        fclose( f_handle->f_p );
        f_handle->f_p = fopen( fname, "r+b" );
    }
    return 0;
}

int ik_fs_read( IK_FILE_HANDLE *f_handle, int offset, uint8_t *buf, uint32_t size )
{
    fseek( f_handle->f_p, offset, SEEK_SET );
    fread( buf, size, 1, f_handle->f_p );
    return 0;
}

int ik_fs_write( IK_FILE_HANDLE *f_handle, int offset, uint8_t *buf, uint32_t size )
{
    fseek( f_handle->f_p, offset, SEEK_SET );
    fwrite( buf, size, 1, f_handle->f_p );
    return 0;
}

int ik_fs_close( IK_FILE_HANDLE *f_handle )
{
    fclose( f_handle->f_p );
    return 0;
}

int ik_fs_ls( uint8_t *buf, uint32_t size )
{
    return 0;
}
