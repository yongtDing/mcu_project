/*************************************************************************
	> File Name: car_driver.h
	> Author: ma6174
	> Mail: ma6174@163.com 
	> Created Time: 2022/11/1 12:23:07
 ************************************************************************/

#ifndef __CAR_DRIVER_H__
#define __CAR_DRIVER_H__

#include "gd32e10x.h"
#include "74hc4051bq.h"

#define MOTO_0_A(a)	if (a)	\
    gpio_bit_set(GPIOB, GPIO_PIN_0);\
    else		\
    gpio_bit_reset(GPIOB, GPIO_PIN_0)						

#define MOTO_0_B(a)	if (a)	\
    gpio_bit_set(GPIOB, GPIO_PIN_1);\
    else		\
    gpio_bit_reset(GPIOB, GPIO_PIN_1)						

#define MOTO_1_A(a)	if (a)	\
    gpio_bit_set(GPIOB, GPIO_PIN_2);\
    else		\
    gpio_bit_reset(GPIOB, GPIO_PIN_2)	

#define MOTO_1_B(a)	if (a)	\
    gpio_bit_set(GPIOB, GPIO_PIN_3);\
    else		\
    gpio_bit_reset(GPIOB, GPIO_PIN_3)						

typedef enum {
    CAR_ADVANCE_MODE,
    CAR_BACK_MODE,
    CAR_LEFT_MODE,
    CAR_RIGHT_MODE,
    CAR_STOP_MODE
} CAR_DRIVER_MODE_T;

void car_bumper_cal(process_handle_t *process_handle);
void car_driver_init(void);
void car_driver_set_run_mode(CAR_DRIVER_MODE_T car_mode);

#endif

