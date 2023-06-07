#ifndef __TIMER_h
#define __TIMER_h

#include "gd32e10x.h"
#include "string.h"
#include "systick.h"
#include "stdbool.h"
void timer_config(uint32_t timer_periph, uint32_t time_interval_100us);
void timer_config2(uint32_t timer_periph, uint32_t time_interval_100us);

#endif
