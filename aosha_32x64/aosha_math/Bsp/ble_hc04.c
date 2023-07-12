/*************************************************************************
	> File Name: ble_hc04.c
	> Author: ma6174
	> Mail: ma6174@163.com 
	> Created Time: 2023/7/11 16:13:17
 ************************************************************************/

#include "uart.h"
#include "ble_hc04.h"
#include "at_commit_base.h"
#include "74hc4051bq.h"

#define BLE_LINK_GET \
    gpio_input_bit_get(GPIOD, GPIO_PIN_2)

void ble_hc04_gpio_init()
{
    rcu_periph_clock_enable(RCU_GPIOD);
    gpio_init(GPIOD, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_2MHZ, GPIO_PIN_2);
}

void ble_hc04_init(USART_COM_ID_T com)
{
    at_config_at_cmd_timeout(com, "AT+BAUD=460800", NULL, 0, "", 500);
    at_config_at_cmd_timeout(com, "AT+NAME=MXSPP_SENSOR", NULL, 0, "", 500);
    at_config_at_cmd_timeout(com, "AT+BNAME=MX_BLE_SENSOR_001", NULL, 0, "", 500);
    at_config_at_cmd_timeout(com, "AT+CINT=6", NULL, 0, "", 500);
    at_config_at_cmd_timeout(com, "AT+RESET", NULL, 0, "", 500);
    delay_1ms(1000);
}

void ble_hc04_thread_loop(void *handle)
{
    process_handle_t *process_handle = (process_handle_t *)handle;

    if (BLE_LINK_GET == 0)
    {
        process_handle->ble_link_state = 1;
        process_handle->upload_interval = 100;
    } else {
        process_handle->ble_link_state = 0;
        process_handle->upload_interval = 50;
    }
}
