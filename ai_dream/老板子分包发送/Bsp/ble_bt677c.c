/*************************************************************************
	> File Name: ble_bt677c.c
	> Author: ma6174
	> Mail: ma6174@163.com 
	> Created Time: 2023/6/29 11:29:12
 ************************************************************************/

#include "uart.h"
#include "ble_bt677c.h"
#include "at_commit_base.h"
#include "aid_agreement.h"


#define CMD_LEN_E104 64

#define BLE_LINK_GET \
    gpio_input_bit_get(GPIOB, GPIO_PIN_3)

#define BLE_MODE_AT \
    gpio_bit_reset(GPIOB, GPIO_PIN_5)

#define BLE_MODE_UART \
    gpio_bit_set(GPIOB, GPIO_PIN_5)

#define BLE_RESET_E104 \
    gpio_bit_reset(GPIOC, GPIO_PIN_12);\
    delay_1ms(500);\
    gpio_bit_set(GPIOC, GPIO_PIN_12);


void ble_bt677c_gpio_init(USART_COM_ID_T com)
{
    gpio_init(GPIOB, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_3);
    gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_5);
    gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_12);
    BLE_RESET_BT677C;
    delay_1ms(2000);

    if(!usart_rx_probe(com))
    {
        cache_len = usart_get_rx_data_count(com);
        usart_recv(com, cache_buffer, cache_len);
    }

}

void ble_bt677c_init(USART_COM_ID_T com)
{
    char cmd_recv[CMD_LEN_E104] = {0};
    char cmd_send[CMD_LEN_E104] = {0};
    
    BLE_RESET_E104;
    BLE_MODE_AT;
    delay_1ms(500);
    at_config_at_cmd_timeout(com, "at+reset", NULL, 0, "+OK", 200);
    delay_1ms(500);
    at_consult_at_cmd(com, "at+mac?", cmd_recv, CMD_LEN_E104, "+OK=");
    reverse(cmd_recv);
    strcat(cmd_send, "at+advdat=");
    strcat(cmd_send, cmd_recv);
    at_config_at_cmd_timeout(com, cmd_send, NULL, 0, "+OK", 200);
    at_config_at_cmd_timeout(com, "at+reset", NULL, 0, "+OK", 0);
    delay_1ms(500);

    BLE_MODE_UART;
}

void ble_bt677c_thread_task(void *process_handle, void *value, uint16_t value_len)
{
    aid_agreement_context_t *aid_agreement_context
        = (aid_agreement_context_t *)process_handle;
    
    if (BLE_LINK_GET == SET)
    {
        aid_mx_set_auto_ack(aid_agreement_context, true);
    } else {
        aid_mx_set_auto_ack(aid_agreement_context, false);
    }
    {
        aid_mx_value_send_raw(aid_agreement_context,
                (uint8_t *)value,
                value_len);
    }

}

