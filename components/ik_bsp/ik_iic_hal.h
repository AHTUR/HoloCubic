#ifndef _IK_IIC_HAL_H
#define _IK_IIC_HAL_h

#include "stdint.h"

typedef struct 
{
    int (*open)( void );
    int (*close)( void );
    int (*transfer)( uint8_t *tx_buf, uint32_t tx_len, uint8_t *rx_buf, uint32_t rx_len );
} IK_IIC_MASTER_HAL_STRUCT;

extern IK_IIC_MASTER_HAL_STRUCT ik_iic;

#endif // !_IK_IIC_HAL_H
