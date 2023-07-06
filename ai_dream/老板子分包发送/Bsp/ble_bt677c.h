/*************************************************************************
	> File Name: ble_bt677c.h
	> Author: ma6174
	> Mail: ma6174@163.com 
	> Created Time: 2023/6/29 11:29:20
 ************************************************************************/

#ifndef __BLE_BT67C_H__
#define __BLE_BT67C_H__

void ble_bt677c_gpio_init(USART_COM_ID_T com);

void ble_bt677c_init(USART_COM_ID_T com);

void ble_bt677c_thread_task(void *process_handle, void *value, uint16_t value_len);

#endif

