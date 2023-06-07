#ifndef __MPU6050_H__
#define __MPU6050_H__

#include "gd32e10x.h"
#include "string.h"
#include "systick.h"
#include "stdbool.h"

#define u8 uint8_t

void IIC_GPIO_Init(void);
int IIC_WriteData(u8 dev_addr,u8 reg_addr,u8 data);
int IIC_ReadData(u8 dev_addr,u8 reg_addr,u8 *pdata,u8 count);
void HEAT_Configuration(void);
void WriteCmd(u8 command);
void WriteDat(u8 data);

#endif
