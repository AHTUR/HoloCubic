#ifndef _IK_FS_PORT_H
#define _IK_FS_PORT_H

#include "stdint.h"

typedef struct 
{
    void *f_p;
}IK_FILE_HANDLE;

int ik_fs_mount( void );
int ik_fs_unmount( void );
int ik_fs_format( void );
int ik_fs_open( char *fname, IK_FILE_HANDLE *f_handle );
int ik_fs_read( IK_FILE_HANDLE *f_handle, int offset, uint8_t *buf, uint32_t size );
int ik_fs_write( IK_FILE_HANDLE *f_handle, int offset, uint8_t *buf, uint32_t size );
int ik_fs_close( IK_FILE_HANDLE *f_handle );
int ik_fs_ls( uint8_t *buf, uint32_t size );

#endif // !_IK_FS_PORT_H
