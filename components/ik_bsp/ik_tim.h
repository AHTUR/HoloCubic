#ifndef _IK_TIM_H
#define _IK_TIM_H

#include "stdint.h"

uint64_t ik_tim_get_tick_ms( void );
void ik_tim_init( void );
void ik_tim_deinit( void );

#endif // !_IK_TIM_H
