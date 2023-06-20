/*************************************************************************
  > File Name: ble_type_E104.c
  > Author: ma6174
  > Mail: ma6174@163.com 
  > Created Time: 2023/1/28 14:39:45
 ************************************************************************/

#include "uart.h"
#include "ble_type_E104_BT53.h"

#define CMD_LEN_E104 64


void reverse(char *str)
{
    int len = strlen(str);
    char *p1 = str;
    char *p2 = str + len - 1;

    while(p1 < p2) {
        char temp = *p1;
        *p1 = *p2;
        *p2 = temp;
        p1++;
        p2--;
    }
}

void ble_consult_at_cmd(USART_COM_ID_T com, char *input_cmd, char *output_cmd, uint16_t output_cmd_len)
{
    uint8_t cache_len = 0;
    memset(output_cmd, 0x00, output_cmd_len);
    usart_dma_send_data(com, (uint8_t *)input_cmd, strlen((char *)input_cmd));
    while (usart_rx_probe(com));
    {
        cache_len = usart_get_rx_data_count(com);
        usart_recv(com, (uint8_t *)output_cmd, cache_len);
    }

    if (strncmp("+OK=", output_cmd, strlen("+OK=")) == 0)
    {
        strcpy(output_cmd, output_cmd + strlen("+OK="));
        printf("%s\n", output_cmd);
    }
}



void ble_config_at_cmd(USART_COM_ID_T com, char *cmd, char *value)
{
    char cmd_send[CMD_LEN_E104] = {0};
    char cmd_recv[CMD_LEN_E104] = {0};
    uint8_t cache_len = 0;

    strcat(cmd_send, cmd);
    strcat(cmd_send, value);
    usart_dma_send_data(com, (uint8_t *)cmd_send, strlen(cmd_send));
    while (usart_rx_probe(com));
    {
        cache_len = usart_get_rx_data_count(com);
        usart_recv(com, (uint8_t *)cmd_recv, cache_len);
    }

    if (strncmp("+OK", cmd_recv, strlen("+OK")) == 0)
    {
        printf("set %s%s success!", cmd, value);
    }
}

void E104_bt53_init(USART_COM_ID_T com)
{
    char cmd_recv[CMD_LEN_E104] = {0};
    
    gpio_init(GPIOB, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_3);
    gpio_init(GPIOB, GPIO_MODE_OUT_OD, GPIO_OSPEED_2MHZ, GPIO_PIN_5);
    gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_12);
    delay_1ms(500);

    BLE_RESET_E104;
    BLE_MODE_AT;
    delay_1ms(500);
    ble_config_at_cmd(com, "at+reset\r\n", "");
    delay_1ms(500);
//    ble_consult_at_cmd(com, "at+mac?", cmd_recv, CMD_LEN_E104);
//    reverse(cmd_recv);
    ble_config_at_cmd(com, "AT+ADVDATA=1,8CF681449CD7\r\n", cmd_recv);
	ble_config_at_cmd(com, "AT+CONN_INTERVAL=6\r\n", cmd_recv);
    ble_config_at_cmd(com, "at+reset\r\n", "");
    delay_1ms(500);

    BLE_MODE_UART;
	;
}

