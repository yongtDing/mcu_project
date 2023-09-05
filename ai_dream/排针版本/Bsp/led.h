/*************************************************************************
	> File Name: led.h
	> Author: ma6174
	> Mail: ma6174@163.com 
	> Created Time: 2022/11/1 12:49:54
 ************************************************************************/

#ifndef __LED_H__
#define __LED_H__

#include "gd32e10x.h"

#define GREEN_LED(a)	if (a)	\
    gpio_bit_set(GPIOB, GPIO_PIN_1);\
    else		\
    gpio_bit_reset(GPIOB, GPIO_PIN_1)						

		
#define RED_EXT_LED(a)	if (a)	\
gpio_bit_set(GPIOB, GPIO_PIN_4);\
else		\
gpio_bit_reset(GPIOB, GPIO_PIN_4)						


#define GREEN_EXT_LED(a)	if (a)	\
gpio_bit_set(GPIOB, GPIO_PIN_5);\
else		\
gpio_bit_reset(GPIOB, GPIO_PIN_5)						

void led_init(void);

#endif

