#ifndef __ADC_h
#define __ADC_h

#include "gd32e10x.h"
#include "string.h"
#include "systick.h"
#include "stdbool.h"

void rcu_config(void);
void gpio_config(void);
void dma_config(void);
void adc_config(void);
void adc_init(void);
void read_adc_value(void);
float GetAdcValue(uint8_t index);
#endif

