#include "ik_tim.h"
#include "driver/gptimer.h"

static gptimer_handle_t gptimer = NULL;
static gptimer_config_t timer_config = {
    .clk_src = GPTIMER_CLK_SRC_DEFAULT,
    .direction = GPTIMER_COUNT_UP,
    .resolution_hz = 1 * 1000 * 1000, // 1MHz, 1 tick = 1us
};

uint64_t ik_tim_get_tick_us( void )
{
    uint64_t count = 0;
    gptimer_get_raw_count(gptimer, &count);
    return count;
}

uint64_t ik_tim_get_tick_ms( void )
{
    uint64_t count = 0;
    count = ik_tim_get_tick_us();
    return ( count / 1000 );
}

void ik_tim_init( void )
{
    gptimer_new_timer(&timer_config, &gptimer);
    gptimer_enable(gptimer);
    gptimer_start(gptimer);
}

void ik_tim_deinit( void )
{
    gptimer_stop(gptimer);
    gptimer_disable(gptimer);
    gptimer_del_timer(gptimer);
}
