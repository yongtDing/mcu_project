/*************************************************************************
	> File Name: ble_hc04.h
	> Author: ma6174
	> Mail: ma6174@163.com 
	> Created Time: 2023/7/11 16:13:26
 ************************************************************************/

#ifndef __BLE_HC04_H__
#define __BLE_HC04_H__

void ble_hc04_gpio_init(void);
void ble_hc04_init(USART_COM_ID_T com);
void ble_hc04_thread_loop(void *handle);

#endif

