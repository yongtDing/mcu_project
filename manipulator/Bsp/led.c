/*************************************************************************
	> File Name: led.c
	> Author: ma6174
	> Mail: ma6174@163.com 
	> Created Time: 2022/11/1 12:49:51
 ************************************************************************/

#include "led.h"
#include "systick.h"

void led_init(void)
{
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_AF);
    gpio_pin_remap_config(GPIO_SWJ_SWDPENABLE_REMAP, ENABLE);
    gpio_init(GPIOA, GPIO_MODE_OUT_OD, GPIO_OSPEED_10MHZ,  GPIO_PIN_15);

    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_AF);
    gpio_pin_remap_config(GPIO_SWJ_SWDPENABLE_REMAP, ENABLE);
    gpio_init(GPIOB, GPIO_MODE_OUT_OD, GPIO_OSPEED_10MHZ,  GPIO_PIN_5);
    BUZZER_ENABLE(1);

}

void buzzer_run(uint8_t tick_num)
{
    uint8_t tick_value = 0;
    for (tick_value = 0; tick_value < tick_num; tick_value ++)
    {
        BUZZER_ENABLE(0);
        delay_1ms(100);
        BUZZER_ENABLE(1);
        delay_1ms(100);
    }

}

void buzzer_run_long(uint8_t tick_num)
{
    uint8_t tick_value = 0;
    for (tick_value = 0; tick_value < tick_num; tick_value ++)
    {
        BUZZER_ENABLE(0);
        delay_1ms(500);
        BUZZER_ENABLE(1);
        delay_1ms(500);
    }

}

