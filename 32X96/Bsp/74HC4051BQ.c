#include <stdlib.h>
#include <stdio.h>
#include "74hc4051bq.h"
#include "adc.h"
#include "math.h"
#include "string.h"
#include "oled.h"


#define ADC_DELAY_TIME (18 * 120) //9 ns * 120

void Hc4051Delay(uint32_t num)
{
    while(num --);
}


void Hc4051IoInit(void)
{
    rcu_periph_clock_enable(RCU_GPIOB); 
    rcu_periph_clock_enable(RCU_GPIOD);
    rcu_periph_clock_enable(RCU_GPIOE);
    rcu_periph_clock_enable(RCU_AF);
    gpio_pin_remap_config(GPIO_SWJ_SWDPENABLE_REMAP, ENABLE);

    gpio_init(GPIOD, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_ALL);
    gpio_init(GPIOE, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_ALL);

    gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ,  GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12);

    XC_A(1);
    XC_B(0);
    XC_C(0);
}

uint8_t adc_rank[12] = {3, 6, 5, 4, 7, 8 ,9, 2, 1, 0, 11, 10};
uint8_t type_a_rank[8] = {6, 5, 4, 7, 0, 3, 1, 2};
uint8_t type_b_rank[8] = {5, 6, 7, 4, 3, 0, 2, 1};

uint8_t adc_channel_choice_num[8] = {6, 5, 4, 7, 3, 0, 2, 1};

void adc_value_read(process_handle_t *process_handle, uint8_t count_y, ADC_MODE_TYPE_T mode_type)
{
    uint8_t x_array = 0;
    uint8_t count = 0, count_1 = 0;

    switch (mode_type)
    {
        case SINGLE_VCC_READ:
            for (count = 0; count < 8; count ++)
            {
                select_x_control(count);
                Hc4051Delay(ADC_DELAY_TIME);
                for (count_1 = 0; count_1 < 12; count_1 ++)
                {
                    x_array = count_1 * 8;
                    x_array += adc_channel_choice_num[count];

                    process_handle->adc_raw_value[count_y][x_array] = GetAdcValue(adc_rank[count_1]);

                }
            }
            break;

        case TOTAL_VCC_READ:
            for (count = 0; count < 8; count ++)
            {
                select_x_control(count);
                Hc4051Delay(ADC_DELAY_TIME);
                for (count_1 = 0; count_1 < 8; count_1 ++)
                {
                    x_array = count_1 * 8;
                    if (count_1 == 1 || count_1 == 2 || count_1 == 5 || count_1 == 6) {
                        x_array += type_b_rank[count];
                    } else {

                        x_array += type_a_rank[count];
                    }

                    process_handle->adc_total_vcc_value[x_array] = GetAdcValue(adc_rank[count_1]);

                }
            }
            break;

        default:
            break;
    }

}

void select_x_control(uint8_t x_value)
{
    uint16_t x_default = 0x00;
    uint16_t hardware_pint = 0;

    x_default = gpio_output_port_get(GPIOA);

    x_default &= 0xe3ff;

    hardware_pint |= (x_value & 0x01) << 2;
    hardware_pint |= (x_value & 0x02);
    hardware_pint |= (x_value & 0x04) >> 2;

    x_default |= (hardware_pint << 10);

    gpio_port_write(GPIOA, x_default);
}

uint8_t refine_y_pos_normal[32] = {22, 21, 20, 19, 18, 17, 16, 7,
    6,  5,  4,  3, 2, 1,  0,  15,
    14,  13,  12,  11,  10, 9, 8, 23,
    24, 25, 26, 27, 28, 29, 30, 31};
//    30, 29, 28, 27, 26, 25, 24, 23};

uint8_t refine_y_pos_aosha[32] = {19, 18, 17, 16, 31, 30, 29, 20,
    21,  0,  1,  2, 10, 11,  9,  8,
    12, 13, 14, 15, 22, 23, 24, 25,
    26, 27, 28,  3,  4,  5,  6,  7};

void select_y_control_volt(process_handle_t *process_handle, uint8_t y_value, ADC_MODE_TYPE_T adc_mode)
{
    uint32_t y_target_value = 0x01;

    switch (adc_mode)
    {
        case SINGLE_VCC_READ:
            y_value = refine_y_pos_normal[y_value];

            if (y_value > 15) {
                y_value -= 16;
                y_target_value = y_target_value << y_value;
                gpio_port_write(GPIOD, 0x00);
                gpio_port_write(GPIOE, y_target_value);
            } else {
                y_target_value = y_target_value << y_value;
                gpio_port_write(GPIOD, y_target_value);
                gpio_port_write(GPIOE, 0x00);
            }
            break;

        case TOTAL_VCC_READ:
            gpio_port_write(GPIOE, 0xffff);
            gpio_port_write(GPIOD, 0xffff);
            break;

        default:

            break;
    }
}

void adc_calculation_calibration_once(process_handle_t *process_handle,
                                      VISUAL_DISPLAY_MODE display_mode)
{
    uint8_t count_x = 0, count_y = 0;
    uint32_t adc_value_2 = 0;

    switch(display_mode)
    {
        case PROCESS_OUT_MODE:
            for (count_y = 0; count_y < SENSOR_POS_Y; count_y ++)
            {
                for (count_x = 0; count_x < SENSOR_POS_X; count_x ++)
                {
                    process_handle->adc_total_vcc_value[count_y] += process_handle->adc_raw_value[count_x][count_y];
                }

                if (process_handle->adc_total_vcc_value[count_y] >=  4096)
                    process_handle->adc_total_vcc_value[count_y] = 4096; 
            }

            for (count_y = 0; count_y < SENSOR_POS_Y; count_y ++)
            {
                for (count_x = 0; count_x < SENSOR_POS_X; count_x ++)
                {
                    adc_value_2 = process_handle->adc_raw_value[count_x][count_y];
                    if (adc_value_2 < 112) {
                        process_handle->adc_cali_value[count_x][count_y] = 0;
                    } else {
                        process_handle->adc_cali_value[count_x][count_y]
                            = 255 * adc_value_2 / (4096 - (process_handle->adc_total_vcc_value[count_y] - adc_value_2));
                    }

                }
            }

            memset(process_handle->adc_total_vcc_value, 0x00, 4 * SENSOR_POS_Y);

            break;

        case RAW_MODE:

            for (count_y = 0; count_y < SENSOR_POS_Y; count_y ++)
            {
                for (count_x = 0; count_x < SENSOR_POS_X; count_x ++)
                {
                    process_handle->adc_cali_value[count_x][count_y] = process_handle->adc_raw_value[count_x][count_y] / 4096.0 * 255;
                    if (process_handle->adc_cali_value[count_x][count_y] <= 7)
                    {
                        process_handle->adc_cali_value[count_x][count_y] = 0;
                    }
                }
            }

            break;

        default:
            break;
    }
}
