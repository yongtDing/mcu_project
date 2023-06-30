/*************************************************************************
  > File Name: led.c
  > Author: ma6174
  > Mail: ma6174@163.com 
  > Created Time: 2022/11/1 12:49:51
 ************************************************************************/

#include "led.h"

void led_init(void)
{
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_AF);
    gpio_pin_remap_config(GPIO_SWJ_SWDPENABLE_REMAP, ENABLE);
    gpio_init(GPIOB, GPIO_MODE_OUT_OD, GPIO_OSPEED_10MHZ,  GPIO_PIN_2);

    GREEN_LED(1);

}

