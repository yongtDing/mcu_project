#include <stdlib.h>
#include <stdio.h>
#include "74hc4051bq.h"
#include "adc.h"
#include "math.h"
#include "string.h"
#include "oled.h"


#define ADC_DELAY_TIME (13 * 220) //9 ns * 120

void rs2251Delay(uint32_t num)
{
    while(num --);
}

void rs2251IoInit(void)
{
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_AF);
    gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ,  GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9);
}

uint8_t adc_rank[16] = { 9,  8, 15, 14,
                         7,  6,  5,  4,
                         3,  2,  1,  0,
                        13, 12, 11, 10};

uint8_t adc_rank_value[16] = {11,  10,  9,  8,
                               7,  6,  5,  4,
                               1,  0, 15, 14,
                              13,  12, 3,  2};


void adc_value_read(process_handle_t *process_handle, uint8_t count_y)
{
    uint8_t count = 0, count_rs2251 = 0;
    uint16_t adc_value = 0;
#if 0
    sprintf(process_handle->printf_buffer, "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n", GetAdcValue(0),
                                                   GetAdcValue(1),
                                                   GetAdcValue(2),
                                                   GetAdcValue(3),
                                                   GetAdcValue(4),
                                                   GetAdcValue(5),
                                                   GetAdcValue(6),
                                                   GetAdcValue(7),
                                                   GetAdcValue(8),
                                                   GetAdcValue(9),
                                                   GetAdcValue(10),
                                                   GetAdcValue(11),
                                                   GetAdcValue(12),
                                                   GetAdcValue(13),
                                                   GetAdcValue(14),
                                                   GetAdcValue(15)
                                                   );

    return ;
#endif
    for (count_rs2251 = 0; count_rs2251 < 8; count_rs2251 ++)
    {
        select_x_control(count_rs2251);
        rs2251Delay(ADC_DELAY_TIME);
        for (count = 0; count < 16; count ++)
        {
            adc_value  = GetAdcValue(count) / 16;
            if (adc_value > 254)
                adc_value = 254;

            process_handle->adc_raw_value[count_y][adc_rank_value[count] * 8 + count_rs2251]
                = adc_value;
        }
    }
}

uint8_t single_adc_rank[8] = {2, 1, 0, 3, 5, 7, 6, 4};
void select_x_control(uint8_t x_value)
{
    uint16_t y_default = 0x00;

    y_default = gpio_output_port_get(GPIOC);

    y_default &= 0xfc7f;

    y_default |= (single_adc_rank[x_value] << 7);

    gpio_port_write(GPIOC, y_default);
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

