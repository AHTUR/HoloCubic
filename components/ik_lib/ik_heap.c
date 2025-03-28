#include "ik_heap.h"
#include "esp_log.h"
#include <malloc.h>

static void* ik_malloc( uint32_t size )
{
    return malloc( size );
}

static void ik_free( void *ptr )
{
    free( ptr );
    return;
}

IK_HEAP_STRUCT ik_heap = {
    .malloc = ik_malloc,
    .free = ik_free,
};