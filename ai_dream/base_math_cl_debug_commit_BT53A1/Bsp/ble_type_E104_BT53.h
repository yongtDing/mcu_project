/*************************************************************************
	> File Name: ble_type_E104.h
	> Author: ma6174
	> Mail: ma6174@163.com 
	> Created Time: 2023/1/28 14:40:01
 ************************************************************************/

#ifndef __BLE_TYPE_H__
#define __BLE_TYPE_H__
#include "uart.h"

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

void E104_bt53_init(USART_COM_ID_T com);

#endif
