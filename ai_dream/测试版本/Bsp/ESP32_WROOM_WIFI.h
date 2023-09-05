/*************************************************************************
	> File Name: ESP32_WROOM_WIFI.h
	> Author: ma6174
	> Mail: ma6174@163.com 
	> Created Time: 2023/6/21 15:45:44
 ************************************************************************/
#ifndef __ESP32_WROOM_WIFI_H__
#define __ESP32_WROOM_WIFI_H__

#define WIFI_ENABLE(a)	if (a)	\
    gpio_bit_set(GPIOB, GPIO_PIN_15);\
    else		\
    gpio_bit_reset(GPIOB, GPIO_PIN_15)						

typedef enum WIFI_CONNECT_STATE {
    WIFI_CONNECT_DISABLE,
    WIFI_CONNECT_NOT_CONNECT,
    WIFI_CONNECTING,
    WIFI_CONNECTED,
} WIFI_CONNECT_STATE_T;

typedef enum WIFI_CONFIG_STATE {
    WIFI_CONFIG_FORCE,
    WIFI_CONFIG_TRY,
} WIFI_CONFIG_STATE_T;


#define WIFI_AP_CACHE_LEN 32

typedef struct wifi_connect_config {
    char wifi_ap_name[WIFI_AP_CACHE_LEN];
    char wifi_ap_password[WIFI_AP_CACHE_LEN];
} wifi_connect_config_t;

typedef struct wifi_context {
    USART_COM_ID_T wifi_com;
    WIFI_CONNECT_STATE_T connect_state;
    wifi_connect_config_t wifi_connect_config;
} wifi_context_t;


void wifi_esp32_gpio_init(void);
void wifi_esp32_wroom_init(USART_COM_ID_T com);
void wifi_config_thread(void *wifi_handle);
int32_t wifi_config_ap(char *ap_name,
                       char *ap_pswd,
                       WIFI_CONFIG_STATE_T config_state);

#endif

