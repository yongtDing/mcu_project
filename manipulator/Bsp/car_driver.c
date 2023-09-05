/*************************************************************************
  > File Name: car_driver.c
  > Author: ma6174
  > Mail: ma6174@163.com 
  > Created Time: 2022/11/1 12:22:44
 ************************************************************************/

#include "car_driver.h"


void car_driver_init()
{
    rcu_periph_clock_enable(RCU_GPIOB);
    gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0 |
            GPIO_PIN_1 |
            GPIO_PIN_2 |
            GPIO_PIN_3);
    MOTO_0_A(0);
    MOTO_0_B(0);

    MOTO_1_A(0);
    MOTO_1_B(0);

}

void car_bumper_cal(process_handle_t *process_handle)
{
    uint8_t count = 0;
    adc_value_read(process_handle, 0);
    for (count = 0; count < 8; count ++)
    {
        process_handle->adc_target_value[0][count] = process_handle->adc_raw_value[0][count] + 17;
    }

}

void car_driver_set_run_mode(CAR_DRIVER_MODE_T car_mode)
{
    switch(car_mode)
    {
        case CAR_ADVANCE_MODE:
            MOTO_0_A(0);
            MOTO_0_B(1);

            MOTO_1_A(0);
            MOTO_1_B(1);

            break;

        case CAR_BACK_MODE:
            MOTO_0_A(1);
            MOTO_0_B(0);

            MOTO_1_A(1);
            MOTO_1_B(0);

            break;

        case CAR_RIGHT_MODE:
            MOTO_0_A(0);
            MOTO_0_B(0);

            MOTO_1_A(1);
            MOTO_1_B(0);

            break;

        case CAR_LEFT_MODE:
            MOTO_0_A(1);
            MOTO_0_B(0);

            MOTO_1_A(0);
            MOTO_1_B(0);

            break;

        case CAR_STOP_MODE:
            MOTO_0_A(0);
            MOTO_0_B(0);

            MOTO_1_A(0);
            MOTO_1_B(0);

            break;

        default:
            break;
    }
}


