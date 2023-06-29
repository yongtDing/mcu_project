/*************************************************************************
	> File Name: ble_bt677c.h
	> Author: ma6174
	> Mail: ma6174@163.com 
	> Created Time: 2023/6/29 11:29:20
 ************************************************************************/

#ifndef __BLE_BT67C_H__
#define __BLE_BT67C_H__


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


#endif

