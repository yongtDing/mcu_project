/*************************************************************************
	> File Name: at_commit_base.c
	> Author: ma6174
	> Mail: ma6174@163.com 
	> Created Time: 2023/6/21 15:52:21
 ************************************************************************/

#include "uart.h"
#include <stdarg.h>
#include "at_commit_base.h"

#define ENABLE_AT_DEBUG_PRINTF

void at_printf(const char *format, ...)
{
#ifdef ENABLE_AT_DEBUG_PRINTF
    printf(format);
#endif
}

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

void at_consult_at_cmd(USART_COM_ID_T com,
                        char *input_cmd,
                        char *output_cmd,
                        uint16_t output_cmd_len,
                        char *ack_success)
{
    uint8_t cache_len = 0;
    memset(output_cmd, 0x00, output_cmd_len);
    usart_dma_send_data(com, (uint8_t *)input_cmd, strlen((char *)input_cmd));
    while (usart_rx_probe(com));
    {
        cache_len = usart_get_rx_data_count(com);
        usart_recv(com, (uint8_t *)output_cmd, cache_len);
    }

    if (strncmp(ack_success, output_cmd, strlen(ack_success)) == 0)
    {
        strcpy(output_cmd, output_cmd + strlen(ack_success));
        at_printf("%s\n", output_cmd);
    }
}

#define CMD_LEN 64

int32_t at_config_sync_succcess(USART_COM_ID_T com,
                                char *cmd,
                                char *ack_success,
                                uint32_t timeout_ms,
                                uint8_t number_times)
{
    char cmd_send[CMD_LEN] = {0};
    char cmd_recv[CMD_LEN] = {0};
    uint8_t cache_len = 0;
    uint16_t time_count = 0;
    uint8_t number_time_private = 0;

    strcat(cmd_send, cmd);

    for (number_time_private = 0; number_time_private < number_times;)
    {
        usart_dma_send_data(com, (uint8_t *)cmd_send, strlen(cmd_send));
        at_printf("at config set %s\n", cmd_send);
        number_time_private ++;
        do {
            if (time_count >= timeout_ms)
            {
                break;
            }
            delay_1ms(200);
            time_count += 200;
        }
        while (usart_rx_probe(com));

        {
            cache_len = usart_get_rx_data_count(com);
            if (cache_len > 0)
            {
                usart_recv(com, (uint8_t *)cmd_recv, cache_len);

                if (strncmp(ack_success, cmd_recv, strlen(ack_success)) == 0)
                {
                    at_printf("set %s success! ack\n%s", cmd, cmd_recv);
                    return 0;
                } else {
                    at_printf("set %s fail, ack\n%s", cmd, cmd_recv);
                    delay_1ms(1000);
                    time_count += 1000;
                    memset(cmd_recv, 0x00, CMD_LEN);
                }
            } else {
                at_printf("set %s fail, TIMEOUT!!, try once more\n", cmd);
                time_count = 0;
            }
        }
    }

    return -2;
}

int32_t at_config_at_cmd_timeout(USART_COM_ID_T com,
                                 char *cmd,
                                 char *ret_value,
                                 uint16_t ret_len,
                                 char *ack_success,
                                 uint32_t timeout_ms)
{
    char cmd_recv[CMD_LEN] = {0};
    uint8_t cache_len = 0;
    uint16_t time_count = 0;

    usart_dma_send_data(com, (uint8_t *)cmd, strlen(cmd));
    at_printf("at config set %s\n", cmd);
    do {
        if (time_count >= timeout_ms)
        {
            break;
        }
        delay_1ms(200);
        time_count += 200;
    }
    while (usart_rx_probe(com));

    {
        cache_len = usart_get_rx_data_count(com);
        if (cache_len > 0)
        {
            usart_recv(com, (uint8_t *)cmd_recv, cache_len);

            if (strncmp(ack_success, cmd_recv, strlen(ack_success)) == 0)
            {
                at_printf("set %s success! ack\n%s", cmd, cmd_recv);
                return 0;
            } else {
                at_printf("set %s fail, ack\n%s", cmd, cmd_recv);
                return -1;
            }
        } else {
            at_printf("set %s fail, TIMEOUT!!\n", cmd);
            return -2;
        }
    }
}

int32_t at_wait_cmd_ack(USART_COM_ID_T com,
                        char *ack_success,
                        uint32_t timeout_ms)
{
    char cmd_recv[CMD_LEN] = {0};
    uint8_t cache_len = 0;
    uint16_t time_count = 0;
   
    do {
        if (time_count >= timeout_ms)
        {
            break;
        }
        delay_1ms(200);
        time_count += 200;
    }
    while (usart_rx_probe(com));

    {
        cache_len = usart_get_rx_data_count(com);
        if (cache_len > 0)
        {
            usart_recv(com, (uint8_t *)cmd_recv, cache_len);

            if (strncmp(ack_success, cmd_recv, strlen(ack_success)) == 0)
            {
                at_printf("wait ack success! ack\n%s", cmd_recv);
                return 0;
            } else {
                at_printf("wait %s fail, ack\n%s", ack_success, cmd_recv);
                return -1;
            }
        } else {
            at_printf("wait %s fail, TIMEOUT!!\n", ack_success);
            return -2;
        }
    }
}

int32_t at_wait_cmd_delay(USART_COM_ID_T com,
                          char *ack_success,
                          uint32_t timeout_ms)
{
    char cmd_recv[CMD_LEN] = {0};
    uint8_t cache_len = 0;
    uint16_t time_count = 0;
   
    do {
        if (time_count >= timeout_ms)
        {
            break;
        }
        delay_1ms(timeout_ms);
        time_count += timeout_ms;
    }
    while (usart_rx_probe(com));

    {
        cache_len = usart_get_rx_data_count(com);
        if (cache_len > 0)
        {
            usart_recv(com, (uint8_t *)cmd_recv, cache_len);

            if (strncmp(ack_success, cmd_recv, strlen(ack_success)) == 0)
            {
                at_printf("wait ack success! ack\n%s", cmd_recv);
                return 0;
            } else {
                at_printf("wait %s fail, ack\n%s", ack_success, cmd_recv);
                return -1;
            }
        } else {
            at_printf("wait %s fail, TIMEOUT!!\n", ack_success);
            return -2;
        }
    }

}

