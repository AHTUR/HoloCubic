#ifndef _IK_SPI_HAL_H
#define _IK_SPI_HAL_H

#include "stdint.h"

typedef struct 
{
    int max_transfer_size;
    int (*open)( void );
    int (*close)( void );
    int (*transfer)( uint8_t *tx_buf, uint16_t tx_len, uint8_t *rx_buf, uint16_t rx_len );
} IK_SPI_HAL_STRUCT;

extern IK_SPI_HAL_STRUCT ik_spi0;

#endif
