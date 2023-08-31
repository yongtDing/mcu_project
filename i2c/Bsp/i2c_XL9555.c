/*************************************************************************
	> File Name: i2c_XL9555.c
	> Author: ma6174
	> Mail: ma6174@163.com 
	> Created Time: 2023/7/18 17:36:22
 ************************************************************************/

#include "gd32e10x.h"
#include "priv_i2c.h"
#include "i2c_XL9555.h"

#define START_CONFIG_REG 0x06
#define END_CONFIG_REG 0x07

#define START_IO_REG 0x02
#define END_IO_REG 0x03


uint8_t xl9555_rank[8] = {0x40, 0x42, 0x44, 0x46, 0x48, 0x4a, 0x4c, 0x4e};

int8_t xl9555_init(void)
{
    uint8_t count = 0;

    for (count = 0; count < 8; count ++)
    {
        i2c_write_data(I2C_COM0, xl9555_rank[count], START_CONFIG_REG, 0x00);
        i2c_write_data(I2C_COM0, xl9555_rank[count], END_CONFIG_REG, 0x00);

        i2c_write_data(I2C_COM0, xl9555_rank[count], START_IO_REG, 0x00);
        i2c_write_data(I2C_COM0, xl9555_rank[count], END_IO_REG, 0x00);

    }

    return 0;
}

void single_xl9555_io_set(uint8_t io_num)
{
    if (io_num % 16 <= 7)
        i2c_write_data(I2C_COM0, xl9555_rank[io_num / 16], START_IO_REG, 0x01 << (io_num % 8));
    else
        i2c_write_data(I2C_COM0, xl9555_rank[io_num / 16], END_IO_REG, 0x80 >> (io_num % 8));
}

inline void total_xl9555_io_clean(uint8_t io_num)
{
    if (io_num % 16 <= 7)
        i2c_write_data(I2C_COM0, xl9555_rank[io_num / 16], START_IO_REG, 0x00);
    else
        i2c_write_data(I2C_COM0, xl9555_rank[io_num / 16], END_IO_REG, 0x00);
}

void xl9555_io_set(uint8_t io_num, uint8_t value)
{
    static uint8_t gate_num = 0;

    if (io_num > 127)
        return ;

    if (gate_num / 8 != io_num / 8)
    {
        total_xl9555_io_clean(gate_num);
    }

    single_xl9555_io_set(io_num);

    gate_num = io_num;
 
}


