#include "timer.h"

void timer_config(uint32_t timer_periph, uint32_t time_interval_100us)
{
    timer_parameter_struct timer_initpara;

    switch(timer_periph) {
    case TIMER0:
        rcu_periph_clock_enable(RCU_TIMER0);
        break;
    case TIMER1:
        rcu_periph_clock_enable(RCU_TIMER1);
        break;
    case TIMER2:
        rcu_periph_clock_enable(RCU_TIMER2);
        break;
    case TIMER3:
        rcu_periph_clock_enable(RCU_TIMER3);
        break;
    case TIMER4:
        rcu_periph_clock_enable(RCU_TIMER4);
        break;
    case TIMER5:
        rcu_periph_clock_enable(RCU_TIMER5);
        break;
    case TIMER6:
        rcu_periph_clock_enable(RCU_TIMER6);
        break;
    default:
        break;
    }

    timer_deinit(timer_periph);
    timer_struct_para_init(&timer_initpara);
    timer_initpara.prescaler         = 11999;	//120M/12000 = 10K Hz  10000
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = (uint32_t)1 * time_interval_100us;//(uint32_t)1000000U/time_interval_us;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_init(timer_periph, &timer_initpara);

    timer_interrupt_enable(timer_periph, TIMER_INT_UP);	//update interrupt
    timer_enable(timer_periph);
}


void timer_config2(uint32_t timer_periph, uint32_t time_interval_100us)
{
    timer_parameter_struct timer_initpara;

    switch(timer_periph) {
    case TIMER0:
        rcu_periph_clock_enable(RCU_TIMER0);
        break;
    case TIMER1:
        rcu_periph_clock_enable(RCU_TIMER1);
        break;
    case TIMER2:
        rcu_periph_clock_enable(RCU_TIMER2);
        break;
    case TIMER3:
        rcu_periph_clock_enable(RCU_TIMER3);
        break;
    case TIMER4:
        rcu_periph_clock_enable(RCU_TIMER4);
        break;
    case TIMER5:
        rcu_periph_clock_enable(RCU_TIMER5);
        break;
    case TIMER6:
        rcu_periph_clock_enable(RCU_TIMER6);
        break;
    default:
        break;
    }

    timer_deinit(timer_periph);
    timer_struct_para_init(&timer_initpara);
    timer_initpara.prescaler         = 11999;	//120M / 10800 = 1000K Hz  12000
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = (uint32_t)time_interval_100us;//(uint32_t)1000000U/ time_interval_us;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_init(timer_periph, &timer_initpara);

    timer_interrupt_enable(timer_periph, TIMER_INT_UP);	//update interrupt
    timer_enable(timer_periph);
}
