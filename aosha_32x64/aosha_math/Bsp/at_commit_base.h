/*************************************************************************
	> File Name: at_commit_base.h
	> Author: ma6174
	> Mail: ma6174@163.com 
	> Created Time: 2023/6/21 15:53:13
 ************************************************************************/
#ifndef __AT_COMMIT_BASE_H__
#define __AT_COMMIT_BASE_H__

void reverse(char *str);
void at_consult_at_cmd(USART_COM_ID_T com,
                        char *input_cmd,
                        char *output_cmd,
                        uint16_t output_cmd_len,
                        char *ack_success);


int32_t at_config_sync_succcess(USART_COM_ID_T com,
                                char *cmd,
                                char *ack_success,
                                uint32_t timeout_ms,
                                uint8_t number_times);

int at_config_at_cmd_timeout(USART_COM_ID_T com,
                       char *cmd,
                       char *ret_value,
                       uint16_t ret_len,
                       char *ack_success,
                       uint32_t timeout_ms);

int32_t at_wait_cmd_ack(USART_COM_ID_T com,
                        char *ack_success,
                        uint32_t timeout_ms);

int32_t at_wait_cmd_delay(USART_COM_ID_T com,
                          char *ack_success,
                          uint32_t timeout_ms);

#endif

