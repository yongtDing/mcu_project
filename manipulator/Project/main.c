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
   OF SUCH DAMAGE.
   */

#include <stdlib.h>
#include <stdio.h>
#include "gd32e10x.h"
#include "systick.h"
#include "adc.h"
#include "uart.h"
#include "timer.h"
#include "74hc4051bq.h"
#include "flash.h"
#include "main.h"
#include "oled.h"
#include "math_filter.h"
#include "car_driver.h"
#include "led.h"
#include "pid.h"
#include "manipulator.h"

/*!
  \brief      main function
  \param[in]  none
  \param[out] none
  \retval     none
  */

#define POS_X 2
#define POS_Y 4


#define LOG_PRINTF 0

#pragma pack (1)
typedef struct {
    uint16_t sof;
    uint8_t tran_type;
    uint16_t len;
    uint8_t type;
    uint8_t adc_value[POS_X][POS_Y];
    uint16_t checksum;
} SerialFrame_t;

typedef struct {
    uint16_t sof;
    uint8_t tran_type;
    uint16_t len;
    uint8_t type;
    uint8_t adc_value[4];
    uint16_t checksum;
} single_frame_t;

typedef struct {
    uint16_t sof;
    uint16_t len;
    uint8_t adc_value[POS_Y];
    uint32_t crc32_mpeg_2;
} ProductSerialFrame_t;

typedef struct {
    uint8_t id;
    uint8_t cmd;
    uint16_t reg_id;
    uint16_t reg_value;
    uint16_t checkcrc_value;
} moto_frame_t;

#pragma pack ()

bool timer3_interrupt = true;

SerialFrame_t serial_frame;

process_handle_t process_handle = {0};
PID_TypeDef motor_pid;

unsigned short calculateModbusCRC(unsigned char *data, uint16_t length);
uint16_t CalChecksum(uint8_t * data, uint16_t len);
uint32_t do_crc_table(unsigned char *ptr, int32_t len);
volatile uint64_t total_time_1 = 0x00;
#define INIT_POS_MAIN 1000
int main(void)
{
    bool led_flag = false;
    uint64_t time_1ms = 0;
    float pos_value = 0;
    void *usb_handle = NULL;

    process_handle.x_max = POS_X;
    process_handle.y_max = POS_Y;
    systick_config();
    delay_1ms(2000);
    adc_init();
    led_init();

    usart0_init();
    //    usart1_init();

    usb_init(&usb_handle);

    pid_init(&motor_pid);
    motor_pid.f_param_init(&motor_pid,
            PID_Speed,
            50,
            2,
            0,
            0,
            200,
            0,

            0.5,
            0,
            0.0);

    serial_frame.sof = 0x5aa5;
    serial_frame.tran_type = 0x01; 
    serial_frame.len = sizeof(serial_frame) - 2;
    serial_frame.type = 0x01;

#if 1
    buzzer_run(2);
    manipulator_init();
    while (!manipulator_get_init_states(usb_handle))
    {
        delay_1ms(500);
    }
#else
    buzzer_run(3);
#endif

    manipulator_pos_spd_ctrl(100);
    manipulator_moment_ctrl(100);
    while(1)
    {
        time_1ms ++;
        if(time_1ms % 500 == 0)
        {
            led_flag = !led_flag;
            GREEN_LED(led_flag);
        }

        if(time_1ms % 50 == 0)
        {

            adc_value_read(&process_handle, 0);

            memcpy((uint8_t *)serial_frame.adc_value, (uint8_t *)process_handle.adc_raw_value, process_handle.x_max * process_handle.y_max);
            serial_frame.checksum = CalChecksum((uint8_t *)&serial_frame, sizeof(serial_frame) - 2);


            motor_pid.target = (float)process_handle.adc_raw_value[0][4];
            motor_pid.f_cal_pid(&motor_pid, process_handle.adc_raw_value[0][2]);

            if (motor_pid.measure > 1)
            {
                motor_pid.f_pid_reset(&motor_pid, 0.01, 0, 0);
            } else {
                motor_pid.f_pid_reset(&motor_pid, 0.3, 0, 0);
            }

            if (process_handle.adc_raw_value[0][4] <= 2)
            {
                manipulator_pos_ctrl(INIT_POS_MAIN);
                manipulator_spd_ctrl(0, true);
            }
            else {
                pos_value = manipulator_spd_ctrl(-motor_pid.output, false);
            }
#if 1

#else

            sprintf((char *)process_handle.printf_buffer, "target/input %d/%d, output %f pos_value %f\n",
                    (uint16_t)motor_pid.target,
                    process_handle.adc_raw_value[0][2],
                    motor_pid.output,
                    pos_value);
            usb_send_buffer(usb_handle, process_handle.printf_buffer, strlen((char *)process_handle.printf_buffer));
#endif

            //usart0_dma_send_data((uint8_t *)&moto_frame, sizeof(moto_frame_t));
            //usb_send_buffer(usb_handle, (uint8_t *)&moto_frame, sizeof(moto_frame_t));
        }


        if (time_1ms % 2 == 0)
        {
            usb_send_buffer(usb_handle, (uint8_t *)&serial_frame, sizeof(serial_frame));

//            manipulator_pos_ctrl(850 - serial_frame.adc_value[1][0] * 1);

//            sprintf((char *)process_handle.printf_buffer, "pos value = %d\n", 300 - serial_frame.adc_value[0][0]);
//            usb_send_buffer(usb_handle, process_handle.printf_buffer, strlen((char *)process_handle.printf_buffer));
            
//            pos_count = -10;

        }

#if 0
        if(time_1ms % 10 == 0)
        {
            if (!usart0_rx_probe())
            {
                usart0_len = USART0_GetDataCount();
                USART0_Recv((uint8_t *)moto_contorl_rx, usart0_len);
                usb_send_buffer(usb_handle, moto_contorl_rx, usart0_len);
            }
        }
#endif	
        delay_1ms(1);
    }
}

uint16_t CalChecksum(uint8_t * data, uint16_t len)
{
    uint16_t sum = 0;
    for(int i = 0; i < len; ++i)
    {
        sum += data[i];
    }
    return sum;
}

uint16_t swapEndian16(uint16_t value) {
    return (value << 8) | (value >> 8);
}

const uint32_t crc_table[256] = {
    0x00000000, 0x04c11db7, 0x09823b6e, 0x0d4326d9, 0x130476dc, 0x17c56b6b, 0x1a864db2, 0x1e475005,
    0x2608edb8, 0x22c9f00f, 0x2f8ad6d6, 0x2b4bcb61, 0x350c9b64, 0x31cd86d3, 0x3c8ea00a, 0x384fbdbd,
    0x4c11db70, 0x48d0c6c7, 0x4593e01e, 0x4152fda9, 0x5f15adac, 0x5bd4b01b, 0x569796c2, 0x52568b75,
    0x6a1936c8, 0x6ed82b7f, 0x639b0da6, 0x675a1011, 0x791d4014, 0x7ddc5da3, 0x709f7b7a, 0x745e66cd,
    0x9823b6e0, 0x9ce2ab57, 0x91a18d8e, 0x95609039, 0x8b27c03c, 0x8fe6dd8b, 0x82a5fb52, 0x8664e6e5,
    0xbe2b5b58, 0xbaea46ef, 0xb7a96036, 0xb3687d81, 0xad2f2d84, 0xa9ee3033, 0xa4ad16ea, 0xa06c0b5d,
    0xd4326d90, 0xd0f37027, 0xddb056fe, 0xd9714b49, 0xc7361b4c, 0xc3f706fb, 0xceb42022, 0xca753d95,
    0xf23a8028, 0xf6fb9d9f, 0xfbb8bb46, 0xff79a6f1, 0xe13ef6f4, 0xe5ffeb43, 0xe8bccd9a, 0xec7dd02d,
    0x34867077, 0x30476dc0, 0x3d044b19, 0x39c556ae, 0x278206ab, 0x23431b1c, 0x2e003dc5, 0x2ac12072,
    0x128e9dcf, 0x164f8078, 0x1b0ca6a1, 0x1fcdbb16, 0x018aeb13, 0x054bf6a4, 0x0808d07d, 0x0cc9cdca,
    0x7897ab07, 0x7c56b6b0, 0x71159069, 0x75d48dde, 0x6b93dddb, 0x6f52c06c, 0x6211e6b5, 0x66d0fb02,
    0x5e9f46bf, 0x5a5e5b08, 0x571d7dd1, 0x53dc6066, 0x4d9b3063, 0x495a2dd4, 0x44190b0d, 0x40d816ba,
    0xaca5c697, 0xa864db20, 0xa527fdf9, 0xa1e6e04e, 0xbfa1b04b, 0xbb60adfc, 0xb6238b25, 0xb2e29692,
    0x8aad2b2f, 0x8e6c3698, 0x832f1041, 0x87ee0df6, 0x99a95df3, 0x9d684044, 0x902b669d, 0x94ea7b2a,
    0xe0b41de7, 0xe4750050, 0xe9362689, 0xedf73b3e, 0xf3b06b3b, 0xf771768c, 0xfa325055, 0xfef34de2,
    0xc6bcf05f, 0xc27dede8, 0xcf3ecb31, 0xcbffd686, 0xd5b88683, 0xd1799b34, 0xdc3abded, 0xd8fba05a,
    0x690ce0ee, 0x6dcdfd59, 0x608edb80, 0x644fc637, 0x7a089632, 0x7ec98b85, 0x738aad5c, 0x774bb0eb,
    0x4f040d56, 0x4bc510e1, 0x46863638, 0x42472b8f, 0x5c007b8a, 0x58c1663d, 0x558240e4, 0x51435d53,
    0x251d3b9e, 0x21dc2629, 0x2c9f00f0, 0x285e1d47, 0x36194d42, 0x32d850f5, 0x3f9b762c, 0x3b5a6b9b,
    0x0315d626, 0x07d4cb91, 0x0a97ed48, 0x0e56f0ff, 0x1011a0fa, 0x14d0bd4d, 0x19939b94, 0x1d528623,
    0xf12f560e, 0xf5ee4bb9, 0xf8ad6d60, 0xfc6c70d7, 0xe22b20d2, 0xe6ea3d65, 0xeba91bbc, 0xef68060b,
    0xd727bbb6, 0xd3e6a601, 0xdea580d8, 0xda649d6f, 0xc423cd6a, 0xc0e2d0dd, 0xcda1f604, 0xc960ebb3,
    0xbd3e8d7e, 0xb9ff90c9, 0xb4bcb610, 0xb07daba7, 0xae3afba2, 0xaafbe615, 0xa7b8c0cc, 0xa379dd7b,
    0x9b3660c6, 0x9ff77d71, 0x92b45ba8, 0x9675461f, 0x8832161a, 0x8cf30bad, 0x81b02d74, 0x857130c3,
    0x5d8a9099, 0x594b8d2e, 0x5408abf7, 0x50c9b640, 0x4e8ee645, 0x4a4ffbf2, 0x470cdd2b, 0x43cdc09c,
    0x7b827d21, 0x7f436096, 0x7200464f, 0x76c15bf8, 0x68860bfd, 0x6c47164a, 0x61043093, 0x65c52d24,
    0x119b4be9, 0x155a565e, 0x18197087, 0x1cd86d30, 0x029f3d35, 0x065e2082, 0x0b1d065b, 0x0fdc1bec,
    0x3793a651, 0x3352bbe6, 0x3e119d3f, 0x3ad08088, 0x2497d08d, 0x2056cd3a, 0x2d15ebe3, 0x29d4f654,
    0xc5a92679, 0xc1683bce, 0xcc2b1d17, 0xc8ea00a0, 0xd6ad50a5, 0xd26c4d12, 0xdf2f6bcb, 0xdbee767c,
    0xe3a1cbc1, 0xe760d676, 0xea23f0af, 0xeee2ed18, 0xf0a5bd1d, 0xf464a0aa, 0xf9278673, 0xfde69bc4,
    0x89b8fd09, 0x8d79e0be, 0x803ac667, 0x84fbdbd0, 0x9abc8bd5, 0x9e7d9662, 0x933eb0bb, 0x97ffad0c,
    0xafb010b1, 0xab710d06, 0xa6322bdf, 0xa2f33668, 0xbcb4666d, 0xb8757bda, 0xb5365d03, 0xb1f740b4,
};

uint32_t do_crc_table(unsigned char *ptr, int len)
{
    unsigned int crc = 0xFFFFFFFF;

    while(len--)
    {
        crc = (crc << 8) ^ crc_table[(crc >> 24 ^ *ptr++) & 0xff];
    }

    return crc;
}

uint16_t calculateModbusCRC(uint8_t *data, uint16_t length) {
    uint16_t crc = 0xFFFF;

    for (uint16_t i = 0; i < length; i++) {
        crc ^= data[i];

        for (int j = 0; j < 8; j++) {
            if (crc & 0x0001) {
                crc >>= 1;
                crc ^= 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }

    return crc;
}
void CAN0_RX0_IRQHandler(void)
{
    static bool led_flag_red = false;
    led_flag_red = !led_flag_red;
    can_receive_message_struct receive_message;

    /* check the receive message */
    can_message_receive(CAN0, CAN_FIFO0, &receive_message);
}

void TIMER3_IRQHandler( void )
{
    if( SET == timer_interrupt_flag_get( TIMER3, TIMER_INT_FLAG_UP ) )
    {
        /* clear channel 0 interrupt bit */
        timer_interrupt_flag_clear( TIMER3, TIMER_INT_FLAG_UP );

    }
}

int compareArrays(uint8_t *arr1, uint8_t *arr2, int size)
{
    for (int i = 0; i < size; i++) {
        if (arr1[i] != arr2[i]) {
            return 0;
        }
    }

    return 1;
}

#define WINDOW_SIZE 5

uint8_t smoothFilter(uint8_t* data, int dataSize)
{
    uint16_t smoothedValue = 0;
    int i;

    for (i = 0; i < dataSize; i++)
    {
        if (i < WINDOW_SIZE)
        {
            // 前几个数据点直接输出，因为窗口不足时无法计算完整的移动平均
            smoothedValue = data[i];
        }
        else
        {
            // 计算窗口范围内数据点的移动平均
            smoothedValue = 0;
            int j;
            for (j = i - WINDOW_SIZE + 1; j <= i; j++)
            {
                smoothedValue += data[j];
            }
            smoothedValue /= WINDOW_SIZE;
        }
    }

    return 0;
}
