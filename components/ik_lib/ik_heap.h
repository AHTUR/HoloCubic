#ifndef _IK_HEAP_H
#define _IK_HEAP_H

#include "stdint.h"

#define IK_MALLOC(size)  ik_heap.malloc(size)
#define IK_FREE(p)  ik_heap.free(p)

typedef struct 
{
    void* (*malloc)( uint32_t size );
    void (*free)( void *ptr );
} IK_HEAP_STRUCT;

extern IK_HEAP_STRUCT ik_heap;

#endif
