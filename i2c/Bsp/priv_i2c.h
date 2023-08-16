/*************************************************************************
	> File Name: priv_i2c.h
	> Author: ma6174
	> Mail: ma6174@163.com 
	> Created Time: 2023/7/17 16:56:47
 ************************************************************************/

#ifndef __PRIV_I2C_H__
#define __PRIV_I2C_H__

typedef enum I2C_COM {
    I2C_COM0,
    I2C_COM1
} I2C_COM_T;

void i2c_init(I2C_COM_T com, uint32_t speed);
void i2c_thread_task(void);
uint8_t i2c_read_reg(I2C_COM_T com, uint8_t address, uint8_t reg);
int i2c_write_data(I2C_COM_T com, uint8_t address, uint8_t reg, uint8_t value);

#endif

