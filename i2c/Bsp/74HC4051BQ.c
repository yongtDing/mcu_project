#include <stdlib.h>
#include <stdio.h>
#include "74hc4051bq.h"
#include "adc.h"
#include "math.h"
#include "string.h"
#include "oled.h"


#define ADC_DELAY_TIME (5 * 220) //9 ns * 120

void Hc4051Delay(uint32_t num)
{
    while(num --);
}

void Hc4051IoInit(void)
{
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_AF);
    gpio_pin_remap_config(GPIO_SWJ_SWDPENABLE_REMAP, ENABLE);
}

uint8_t adc_rank[8] = {0, 1, 7, 4, 6, 5 ,4, 7};
uint8_t type_a_rank[8] = {6, 5, 4, 7, 0, 3, 1, 2};
uint8_t type_b_rank[8] = {5, 6, 7, 4, 3, 0, 2, 1};
float adc_value_compensate[2][4] = {{1.11, 1.08,  1.26, 1.32},  //x
    {1.09, 1.19,  1.23 , 1.36}}; //y


void adc_value_read(process_handle_t *process_handle, uint8_t count_y)
{
    uint8_t count = 0;
    uint16_t adc_value = 0;

    for (count = 0; count < 8; count ++)
    {
        adc_value  = GetAdcValue(count) * 100;
        if (adc_value > 254)
            adc_value = 254;
       
        process_handle->adc_raw_value[count_y][count] = adc_value;
    }
}

uint8_t switch_x_refine[8] = {4, 6, 7, 5, 2, 1, 0, 3};
void select_x_control(uint8_t x_value)
{
    uint16_t y_default = 0x00;

    y_default = gpio_output_port_get(GPIOB);

    y_default &= 0xffc7;

    y_default
  |= (switch_x_refine[x_value] << 3);

    gpio_port_write(GPIOB, y_default);

}

void select_y_control(process_handle_t *process_handle, uint8_t y_value)
{
    uint16_t y_default = 0x00;
    uint8_t hardware_pint = 0;
    bool control_down_flag = true;

    y_default = gpio_output_port_get(GPIOB);
    y_default &= 0x0ff8;
    switch(y_value)
    {
        case 0: case 1: case 2: case 3:
        case 4: case 5: case 6: case 7:
            control_down_flag = true;
            y_default |= 0x7000;
            break;
        case 8: case 9: case 10: case 11:
        case 12: case 13: case 14: case 15:
            control_down_flag = false;
            y_default |= 0xB000;
            break;
        case 16: case 17: case 18: case 19:
        case 20: case 21: case 22: case 23:
            control_down_flag = false;
            y_default |= 0xD000;
            break;
        case 24: case 25: case 26: case 27:
        case 28: case 29: case 30: case 31:
            control_down_flag = true;
            y_default |= 0xE000;
            break;
        default:
            break;
    }

    if (control_down_flag)
    {
        switch (y_value % 8)
        {
            case 0:
                y_value = 4;
                break;
            case 1:
                y_value = 6;
                break;
            case 2:
                y_value = 7;
                break;
            case 3:
                y_value = 5;
                break;
            case 4:
                y_value = 2;
                break;
            case 5:
                y_value = 1;
                break;
            case 6:
                y_value = 0;
                break;
            case 7:
                y_value = 3;
                break;
            default:
                break;
        }
    }
    else {
        switch (y_value % 8)
        {
            case 0:
                y_value = 5;
                break;
            case 1:
                y_value = 7;
                break;
            case 2:
                y_value = 6;
                break;
            case 3:
                y_value = 4;
                break;
            case 4:
                y_value = 3;
                break;
            case 5:
                y_value = 0;
                break;
            case 6:
                y_value = 1;
                break;
            case 7:
                y_value = 2;
                break;
            default:
                break;
        }
    }

    hardware_pint |= (y_value & 0x01) << 2;
    hardware_pint |= (y_value & 0x02);
    hardware_pint |= (y_value & 0x04) >> 2;

    y_default |= (hardware_pint & 0x7);
    gpio_port_write(GPIOB, y_default);
}

