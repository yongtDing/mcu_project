#ifndef __BLE_TYPE_G75__
#define __BLE_TYPE_G75__

#include "gd32e10x.h"
#include "stdio.h"
#include "string.h"

typedef enum {
	BLE_G75_NULL,
	BLE_G75_BT_NAME,    //蓝牙名称
	BLE_G75_UT_VER,     //蓝牙软硬件版本
	BLE_G75_BT_LINK,    //蓝牙连接状态
	BLE_G75_BT_BINT,    //蓝牙广播间隔读取和设置
	BLE_G75_BT_SCAN,    //搜索附近蓝牙设备
	BLE_G75_BT_DISCONN, //断开指定的蓝牙连接
	BLE_G75_BT_CONN,    //主动连接其他的蓝牙设备
    BLE_G75_RF_POWER,   //无线射频功率的读取和设置
    BLE_G75_UT_RESET,   //重启蓝牙
    BLE_G75_LP_SLEEP,   //蓝牙深度睡眠模式
    BLE_G75_UT_MAC,     //蓝牙MAC地址的读取和设置
    BLE_G75_BT_ADV,     //蓝牙广播状态的读取和设置
    BLE_G75_BT_TRANS,   //蓝牙透传模式与AT模式切换
    BLE_G75_UART_BAUD,  //串口波特率的读取和设置
    BLE_G75_UT_CFGSV,   //将所有修改的参数保存到flash中
    BLE_G75_UT_WKMODE,   //自定义关闭蓝牙透传指令
    BLE_G75_UT_RECOVERY //恢复默认值
} ble_g75_cmd_t;

typedef enum {
    BLE_G75_SET_CFG = 0,
    BLE_G75_GET_CFG
} ble_g75_cfg_t;

typedef struct ble_g75_command_context{
    ble_g75_cmd_t command;
    uint8_t           keyword_cmd_char[24];
    uint8_t           get_keyword[1];
    uint8_t           set_keyword[1];
    uint8_t           cfg_cmd[5][10];
} ble_g75_command_context_t;

const ble_g75_command_context_t ble_g75_cmd_context[] = {
    {BLE_G75_BT_NAME, "AT+BT_NAME", '?', '='},
    {BLE_G75_BT_LINK, "AT+BT_LINK", '?', '='},
    {BLE_G75_BT_SCAN, "AT+BT_SCAN", '?', '='},
    {BLE_G75_BT_DISCONN, "AT+BT_DISCONN", '?', '='},
    {BLE_G75_BT_CONN, "AT+BT_CONN", '?', '='},
    {BLE_G75_UT_VER, "AT+UT_VER", '?', '='},
    {BLE_G75_BT_BINT, "AT+BT_BINT", '?', '='},
    {BLE_G75_RF_POWER, "AT+RF_POWER", '?', '='},
    {BLE_G75_UT_RESET, "AT+UT_RESET", 'N', 'N'},
    {BLE_G75_LP_SLEEP, "AT+LP_SLEEP", '?', '='},
    {BLE_G75_UT_MAC, "AT+UT_MAC", '?', '='},
    {BLE_G75_BT_ADV, "AT+BT_ADV", '?', '='},
    {BLE_G75_BT_TRANS, "AT+BT_TRANS", '?', '='},
    {BLE_G75_UART_BAUD, "AT+UART_BAUD", '?', '='},
    {BLE_G75_UT_CFGSV, "AT+UT_CFGSV", 'N', 'N'},
    {BLE_G75_UT_WKMODE, "AT+UT_WKMODE", '?', '='},
    {BLE_G75_UT_RECOVERY, "AT+UT_RECOVERY", '?', '='}
};

#endif

