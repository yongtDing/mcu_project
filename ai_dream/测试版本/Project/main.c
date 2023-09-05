/*!
  \file  main.c
  \brief ADC0 regular channel with DMA

  \version 2017-12-26, V1.0.0, firmware for GD32E10x
  */

/*
   Copyright (c) 2017, GigaDevice Semiconductor Inc.

   All rights reserved.

   Redistribution and use in source and binary forms, with or without modification,
   are permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.
   3. Neither the name of the copyright holder nor the names of its contributors
   may be used to endorse or promote products derived from this software without
   specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
   IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
   INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
   NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
   WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
   OF SUCH DAMAGEp
   */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "gd32e10x.h"
#include "systick.h"
#include "led.h"
#include "adc.h"
#include "uart.h"
#include "timer.h"
#include "74hc4051bq.h"
#include "flash.h"
#include "main.h"
#include "can.h"
#include "i2c.h"
#include "oled.h"
#include "math.h"
#include "aid_agreement.h"
#include "ble_bt677c.h"
#include "malloc.h"
#include "ai_json.h"
#include "ESP32_WROOM_WIFI.h"

/*!
  \brief      main function
  \param[in]  none
  \param[out] none
  \retval     none
  */

#pragma pack (1)


typedef struct {
    uint16_t sof;
    uint8_t tran_type;
    uint16_t len;
    uint8_t type;
    uint8_t adc_value[SENSOR_POS_X][SENSOR_POS_Y];
    uint16_t checksum;
} SerialFrame_t;

#pragma pack ()

//#define DEBUG_MODE
uint32_t time_1ms = 0;

SerialFrame_t serial_frame;
process_handle_t process_handle = {0};

uint16_t CalChecksum(uint8_t * data, uint16_t len);

int main(void)
{
    bool led_flag = false;
    uint8_t count = 0;
    uint16_t cache_len = 0;
    aid_agreement_context_t *aid_agreement_context = NULL;

    systick_config();
    led_init();
    adc_init();
    Hc4051IoInit();
    aid_cmd_circular_init(&aid_agreement_context);
    my_mem_init(SRAMIN);
#if 0
    nvic_irq_enable( TIMER3_IRQn, 2, 0 );
    timer_config(TIMER3, 1); //20ms
#endif
    usart0_init(460800); //wifi
    usart2_init(460800); //485
    usart3_init(460800); //ble

    //E104_bt5032A_init(USART_3_TR);
//    wifi_esp32_gpio_init();
//    ble_bt677c_gpio_init(USART_3_TR);
    //wifi_esp32_wroom_init(USART_0_TR);
    while( 1 )
    {
        time_1ms ++;
        //LED闪烁

        if(time_1ms % 50 == 0)
        {
            led_flag = !led_flag;
            GREEN_LED(led_flag);

            if(!usart_rx_probe(USART_2_TR))
            {
                memset(process_handle.recv_cache, 0x00, CACHE_BUFFER_SIZE);
                cache_len = usart_get_rx_data_count(USART_2_TR);
                usart_recv(USART_2_TR, process_handle.recv_cache, cache_len);

#if 0
                //aid_create_json(process_handle.recv_cache, cache_len);
                aid_message_match(aid_agreement_context, process_handle.recv_cache, cache_len);
#else
                aid_message_match_char(process_handle.recv_cache, cache_len);
#endif
            }

            if (1)
                if(!usart_rx_probe(USART_3_TR))
                {
                    memset(process_handle.printf_buffer, 0x00, CACHE_BUFFER_SIZE);
                    cache_len = usart_get_rx_data_count(USART_3_TR);
                    usart_recv(USART_3_TR, process_handle.printf_buffer, cache_len);
#ifdef DEBUG_MODE
                    {
                        printf(" recv cmd\n ");
                        usart_dma_send_data(USART_2_TR,
                                (uint8_t *)process_handle.printf_buffer,
                                cache_len);
                        delay_1ms(10);
                        printf(" send cmd\n ");
                    }
#endif
                    aid_message_match(aid_agreement_context, process_handle.printf_buffer, cache_len);
                }

        }

        if (time_1ms % 2000 == 0)
        {            
//            wifi_config_thread(NULL);
        }

        //50Hz速率采集第一快压感数据
        if(time_1ms % 1 == 0)
        {
            //for (count = 0; count < SENSOR_POS_X; count ++)
            {
                adc_value_read(&process_handle, count, SINGLE_VCC_READ);

                if (count > 27)
                {
 
                }

#if 1
                count ++;
                if (count == SENSOR_POS_X)
                {
                    count = 0;
                }
#endif
                select_y_control_volt(&process_handle, count, SINGLE_VCC_READ);

            }
        }

        if (time_1ms % 1000 == 0)
        {
            //printf("board mem used %d%\n", my_mem_perused(SRAMIN));
            //aid_paser_json_demo(NULL, 0);
        }

//        if (time_1ms % 100 == 0)
//        {
//            ble_bt677c_thread_task((void *)aid_agreement_context,
//                    (void *)process_handle.adc_cali_value,
//                    SENSOR_POS_X * SENSOR_POS_Y);
//        }

        if(time_1ms % 100 == 0)
        {
            //数据排列转换
            serial_frame.sof = 0x5aa5;
            serial_frame.tran_type = 0x01;
            serial_frame.len = sizeof(serial_frame) - 2;
            serial_frame.type = 0x01;

            adc_calculation_calibration_once(&process_handle);
            
            memcpy((uint8_t *)serial_frame.adc_value, (uint8_t *)process_handle.adc_cali_value, SENSOR_POS_X * SENSOR_POS_Y);
            serial_frame.checksum = CalChecksum((uint8_t *)&serial_frame, sizeof(serial_frame) - 2);

#ifndef DEBUG_MODE
            printf("%d %d %d %d   %u %u %u %u\n", process_handle.adc_raw_value[31][63],
                                  process_handle.adc_raw_value[30][63],
                                  process_handle.adc_raw_value[29][63],
                                  process_handle.adc_raw_value[28][63],
                                  process_handle.adc_cali_value[31][63],
                                  process_handle.adc_cali_value[30][63],
                                  process_handle.adc_cali_value[29][63],
                                  process_handle.adc_cali_value[28][63]

                                  );
            if (0)
            {
                usart_dma_send_data(USART_2_TR, (uint8_t *)&serial_frame, sizeof(serial_frame));
                usart_dma_send_data(USART_0_TR, (uint8_t *)&serial_frame, sizeof(serial_frame));
            }
#endif
        }
        delay_1ms(1);
    }
}

void TIMER3_IRQHandler(void)
{
    if( SET == timer_interrupt_flag_get(TIMER3, TIMER_INT_FLAG_UP))
    {
        /* clear channel 0 interrupt bit */
        timer_interrupt_flag_clear(TIMER3, TIMER_INT_FLAG_UP);
    }
}
