/*************************************************************************
	> File Name: ESP32_WROOM_WIFI.c
	> Author: ma6174
	> Mail: ma6174@163.com 
	> Created Time: 2023/6/21 15:45:31
 ************************************************************************/

#include "uart.h"
#include "ESP32_WROOM_WIFI.h"
#include "at_commit_base.h"
#include "flash.h"

wifi_context_t wifi_handle = {USART_0_TR, WIFI_CONNECT_DISABLE, "", ""};

#define ENABLE_WIFI_DEBUG_PRINTF

void wifi_printf(const char *format, ...)
{
#ifdef ENABLE_WIFI_DEBUG_PRINTF
    printf(format);
#endif
}

void wifi_load_config_flash(wifi_connect_config_t *config)
{

}

void wifi_save_config_flash(wifi_connect_config_t *config)
{

}

void wifi_esp32_gpio_init(void)
{
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_AF);
    gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ,  GPIO_PIN_15);
    WIFI_ENABLE(0);
    delay_1ms(500);
    WIFI_ENABLE(1);
}

void wifi_esp32_wroom_init(USART_COM_ID_T com)
{
#if 0
    if (at_config_at_cmd_wait_success(com, "ATE0\r\n", "", "\r\nOK\r\n", 5000))
    {
        printf("WIFI INIT FAILED!!\n");
        return;
    }
#endif
    //at_config_sync_succcess(com, "AT+RST\r\n", "\r\nOK\r\n", 2000, 3);

    wifi_handle.wifi_com = com;

    at_wait_cmd_ack(com, "\r\nready", 2000);

    if (at_config_sync_succcess(com, "ATE0\r\n", "\r\nOK\r\n", 3000, 5))
    {
        printf("WIFI INIT FAILED!!\n");
        wifi_handle.connect_state = WIFI_CONNECT_DISABLE;
        return;
    }

    at_config_sync_succcess(com, "AT\r\n", "\r\nOK\r\n", 2000, 3);
    at_config_at_cmd_timeout(com, "AT+CWAUTOCONN=0\r\n", NULL, 0, "\r\nOK\r\n", 1000);
    at_config_at_cmd_timeout(com, "AT+CWMODE=1,0\r\n", NULL, 0, "\r\nOK\r\n", 1000);
    at_config_at_cmd_timeout(com, "AT+CWQAP\r\n", NULL, 0, "", 1000);
    wifi_printf("%s run finish\n", __FUNCTION__);
    wifi_handle.connect_state = WIFI_CONNECT_NOT_CONNECT;
}

#define WIFI_BUFFER_CACHE_LEN 64

int32_t wifi_config_ap(char *ap_name,
                       char *ap_pswd,
                       WIFI_CONFIG_STATE_T config_state)
{
    if (wifi_handle.connect_state != WIFI_CONNECT_NOT_CONNECT)
    {
        goto err_config;
    }

    if (strlen(ap_name) < WIFI_BUFFER_CACHE_LEN)
        strcpy(wifi_handle.wifi_connect_config.wifi_ap_name, ap_name);
    else {
        wifi_printf("wifi ap name too lage %d!", strlen(ap_name));
        goto err_config;
    }

    if (strlen(ap_pswd) < WIFI_BUFFER_CACHE_LEN)
        strcpy(wifi_handle.wifi_connect_config.wifi_ap_password, ap_pswd);
    else {
        wifi_printf("wifi ap password too lage %d!", strlen(ap_pswd));
        goto err_config;
    }

    wifi_handle.connect_state = WIFI_CONNECTING;

    save_config_params((uint8_t *)&wifi_handle.wifi_connect_config, sizeof(wifi_connect_config_t));
    return 0;

err_config:
    return -1;
}

WIFI_CONNECT_STATE_T wifi_esp32_connect_ap()
{
    char send_cache[WIFI_BUFFER_CACHE_LEN] = {0};
    char recv_cache[WIFI_BUFFER_CACHE_LEN] = {0};

    strcat(send_cache, "AT+CWJAP=\"");
    strcat(send_cache, wifi_handle.wifi_connect_config.wifi_ap_name);
    strcat(send_cache, "\",");
    strcat(send_cache, "\"");
    strcat(send_cache, wifi_handle.wifi_connect_config.wifi_ap_password);
    strcat(send_cache, "\"\r\n");
    printf("%s: %s", __FUNCTION__, send_cache);
    wifi_handle.connect_state = WIFI_CONNECTED;

    if (0)
    {

        at_config_at_cmd_timeout(wifi_handle.wifi_com, "AT+CWRECONNCFG=1,20\r\n",
                recv_cache, WIFI_BUFFER_CACHE_LEN,
                "\r\nOK\r\n",
                1000);

        at_config_at_cmd_timeout(wifi_handle.wifi_com, "AT+CWJAP=\"601_2.4G\",\"12345678\"\r\n",
                recv_cache, WIFI_BUFFER_CACHE_LEN,
                "WIFI CONNECTED",
                10000);
        at_wait_cmd_ack(wifi_handle.wifi_com, "WIFI GOT IP", 15000);

        at_config_sync_succcess(wifi_handle.wifi_com, "AT+CIPSTA?\r\n", "+CIPSTA", 5000, 3);
    }

    return WIFI_CONNECTED;
}


void wifi_config_thread(void *handle)
{
    if (wifi_handle.connect_state == WIFI_CONNECTING)
    {
    }
}

