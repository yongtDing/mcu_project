/*************************************************************************
  > File Name: manipulator.c
  > Author: ma6174
  > Mail: ma6174@163.com 
  > Created Time: 2023/8/4 11:09:12
 ************************************************************************/

#include "main.h"
#include "stdbool.h"
#include "manipulator.h"
#include "uart.h"
#include "led.h"

#pragma pack (1)
typedef struct {
    uint8_t id;
    uint8_t cmd;
    uint16_t reg_id;
    uint16_t reg_value;
    uint16_t checkcrc_value;
} moto_frame_t;

typedef struct {
    uint8_t id;
    uint8_t cmd;
    uint16_t adress;
    uint8_t ack_value;
    uint16_t checkcrc_value;
} moto_ack_t;

#pragma pack ()


void manipulator_init(void)
{
    static moto_frame_t moto_frame_mani = {0};
    moto_frame_mani.id = 0x01;
    moto_frame_mani.cmd = 0x06;
    moto_frame_mani.reg_id = swapEndian16(0x0100);
    moto_frame_mani.reg_value = swapEndian16(0x00a5);
    moto_frame_mani.checkcrc_value = calculateModbusCRC((unsigned char *)&moto_frame_mani, sizeof(moto_frame_t) - 2);

    usart0_dma_send_data((uint8_t *)&moto_frame_mani, sizeof(moto_frame_t));

    
}

void manipulator_moment_ctrl(uint16_t moment)
{
    static moto_frame_t moto_frame_mani = {0};
    moto_frame_mani.id = 0x01;
    moto_frame_mani.cmd = 0x06;
    moto_frame_mani.reg_id = swapEndian16(0x0101);
    moto_frame_mani.reg_value = swapEndian16(moment);
    moto_frame_mani.checkcrc_value = calculateModbusCRC((unsigned char *)&moto_frame_mani, sizeof(moto_frame_t) - 2);

    usart0_dma_send_data((uint8_t *)&moto_frame_mani, sizeof(moto_frame_t));
}

void manipulator_pos_spd_ctrl(uint16_t pos_spd)
{
    static moto_frame_t moto_frame_mani = {0};
    moto_frame_mani.id = 0x01;
    moto_frame_mani.cmd = 0x06;
    moto_frame_mani.reg_id = swapEndian16(0x0104);
    moto_frame_mani.reg_value = swapEndian16(pos_spd);
    moto_frame_mani.checkcrc_value = calculateModbusCRC((unsigned char *)&moto_frame_mani, sizeof(moto_frame_t) - 2);

    usart0_dma_send_data((uint8_t *)&moto_frame_mani, sizeof(moto_frame_t));
}

void manipulator_pos_ctrl(uint16_t pos)
{
    static moto_frame_t moto_frame_mani = {0};
    uint16_t usart0_len = 0x00;
    moto_ack_t moto_ack = {0};

    moto_frame_mani.id = 0x01;
    moto_frame_mani.cmd = 0x06;
    moto_frame_mani.reg_id = swapEndian16(0x0103);
    moto_frame_mani.reg_value = swapEndian16(pos);
    moto_frame_mani.checkcrc_value = calculateModbusCRC((unsigned char *)&moto_frame_mani, sizeof(moto_frame_t) - 2);

    usart0_dma_send_data((uint8_t *)&moto_frame_mani, sizeof(moto_frame_t));

    while (usart0_rx_probe())
    {
        usart0_len = USART0_GetDataCount();
        if (usart0_len > 5)
        USART0_Recv((uint8_t *)&moto_ack, usart0_len);
    }
}
#define INIT_POS 1000
float manipulator_pos = INIT_POS;
float manipulator_spd_ctrl(float speed, bool clean_flag)
{

    if (clean_flag == true)
    {
        manipulator_pos = INIT_POS;
        return manipulator_pos;
    }

    manipulator_pos += speed;

    if (manipulator_pos >= INIT_POS)
    {
        manipulator_pos = INIT_POS;
    }

    if (manipulator_pos < 10)
    {
        manipulator_pos = 10;
    }

    manipulator_pos_ctrl((uint16_t)manipulator_pos);

    return manipulator_pos;
}

uint8_t moto_contorl_rx[32] = {0};

int8_t manipulator_get_init_states(void *usb_handle)
{
    static moto_frame_t moto_frame_mani = {0};
    uint16_t usart0_len = 0x00;
    moto_ack_t moto_ack = {0};

    moto_frame_mani.id = 0x01;
    moto_frame_mani.cmd = 0x03;
    moto_frame_mani.reg_id = swapEndian16(0x0200);
    moto_frame_mani.reg_value = swapEndian16(0x0001);
    moto_frame_mani.checkcrc_value = calculateModbusCRC((unsigned char *)&moto_frame_mani, sizeof(moto_frame_t) - 2);

    usart0_dma_send_data((uint8_t *)&moto_frame_mani, sizeof(moto_frame_t));

    delay_1ms(200);

    if (!usart0_rx_probe())
    {
        usart0_len = USART0_GetDataCount();
        USART0_Recv((uint8_t *)&moto_ack, usart0_len);
        usb_send_buffer(usb_handle, (uint8_t *)&moto_ack, usart0_len);

        if (moto_ack.ack_value == 0x01)
        {
            buzzer_run_long(2);
            return 1;
        }
    }

    return 0;
}

