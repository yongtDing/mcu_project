/*************************************************************************
  > File Name: priv_i2c.c
  > Author: ma6174
  > Mail: ma6174@163.com
  > Created Time: 2023/7/17 16:56:40
 ************************************************************************/

#include "gd32e10x.h"
#include "priv_i2c.h"

#define I2C0_OWN_ADDRESS7      0x72

#define I2C0_SLAVE_ADDRESS7    0x42
#define I2C0_SLAVE_1_ADDRESS7    0x40


uint8_t i2c_transmitter[8] = {0x00};

void i2c_init(I2C_COM_T com, uint32_t speed)
{
    switch (com)
    {
        case I2C_COM0:
            /* enable GPIOB clock */
            rcu_periph_clock_enable(RCU_GPIOB);
            /* enable I2C0 clock */
            rcu_periph_clock_enable(RCU_I2C0);
            gpio_init(GPIOB, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_6 | GPIO_PIN_7);
            /* configure I2C clock */
            i2c_clock_config(I2C0, speed, I2C_DTCY_2);
            /* configure I2C address */
            i2c_mode_addr_config(I2C0, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, I2C0_OWN_ADDRESS7);
            /* enable I2C0 */
            i2c_enable(I2C0);

            break;

        default:

            break;
    }
}

uint8_t i2c_read_reg(I2C_COM_T com, uint8_t address, uint8_t reg)
{
    uint32_t i2c_periph = 0x00;
    uint8_t data = 0;

    switch (com)
    {
        case I2C_COM0:
            i2c_periph = I2C0;
            break;
    }
    /* enable acknowledge */
    i2c_ack_config(i2c_periph, I2C_ACK_ENABLE);

    /* wait until I2C bus is idle */
    while(i2c_flag_get(i2c_periph, I2C_FLAG_I2CBSY));

    /* send a start condition to I2C bus */
    i2c_start_on_bus(i2c_periph);
    /* wait until SBSEND bit is set */
    while(!i2c_flag_get(i2c_periph, I2C_FLAG_SBSEND));

    /* send slave address to I2C bus */
    i2c_master_addressing(i2c_periph, address, I2C_TRANSMITTER);
    /* wait until ADDSEND bit is set */
    while(!i2c_flag_get(i2c_periph, I2C_FLAG_ADDSEND));
    /* clear ADDSEND bit */
    i2c_flag_clear(i2c_periph, I2C_FLAG_ADDSEND);

    i2c_data_transmit(i2c_periph, reg);
    /* wait until the TBE bit is set */
    while(!i2c_flag_get(i2c_periph, I2C_FLAG_TBE));

    /* send a start condition to I2C bus */
    i2c_start_on_bus(i2c_periph);
    /* wait until SBSEND bit is set */
    while(!i2c_flag_get(i2c_periph, I2C_FLAG_SBSEND));

    /* send slave address to I2C bus */
    i2c_master_addressing(i2c_periph, address, I2C_RECEIVER);

    /* send a NACK for the last data byte */
    i2c_ack_config(i2c_periph, I2C_ACK_DISABLE);
    /* wait until ADDSEND bit is set */
    while(!i2c_flag_get(i2c_periph, I2C_FLAG_ADDSEND));
    /* clear ADDSEND bit */
    i2c_flag_clear(i2c_periph, I2C_FLAG_ADDSEND);


    while(!i2c_flag_get(i2c_periph, I2C_FLAG_RBNE));
    data = i2c_data_receive(i2c_periph);

    {
        i2c_stop_on_bus(i2c_periph);
        while(I2C_CTL0(i2c_periph) & I2C_CTL0_STOP);
    }

    return data;
}

int i2c_write_data(I2C_COM_T com, uint8_t address, uint8_t reg, uint8_t value)
{
    uint32_t i2c_periph = 0x00;

    switch (com)
    {
        case I2C_COM0:
            i2c_periph = I2C0;
            break;
    }
    /* enable acknowledge */
    i2c_ack_config(i2c_periph, I2C_ACK_ENABLE);

    /* wait until I2C bus is idle */
    while(i2c_flag_get(i2c_periph, I2C_FLAG_I2CBSY));

    /* send a start condition to I2C bus */
    i2c_start_on_bus(i2c_periph);
    /* wait until SBSEND bit is set */
    while(!i2c_flag_get(i2c_periph, I2C_FLAG_SBSEND));

    /* send slave address to I2C bus */
    i2c_master_addressing(i2c_periph, address, I2C_TRANSMITTER);
    /* wait until ADDSEND bit is set */
    while(!i2c_flag_get(i2c_periph, I2C_FLAG_ADDSEND));
    /* clear ADDSEND bit */
    i2c_flag_clear(i2c_periph, I2C_FLAG_ADDSEND);

    i2c_data_transmit(i2c_periph, reg);
    /* wait until the TBE bit is set */
    while(!i2c_flag_get(i2c_periph, I2C_FLAG_TBE));

    i2c_data_transmit(i2c_periph, value);
    /* wait until the TBE bit is set */
    while(!i2c_flag_get(i2c_periph, I2C_FLAG_TBE));

    /* end of i2c write */
    {
        i2c_stop_on_bus(i2c_periph);
        while(I2C_CTL0(i2c_periph) & I2C_CTL0_STOP);
    }

    return 0;
}

int i2c_write_data_process_start_address(I2C_COM_T com, uint8_t address)
{

    return 0;
}

int i2c_write_data_process_write_reg(I2C_COM_T com, uint8_t reg, uint8_t value)
{
    uint32_t i2c_periph = 0x00;

    switch (com)
    {
        case I2C_COM0:
            i2c_periph = I2C0;
            break;
    }

    return 0;
}

int i2c_write_data_process_stop(I2C_COM_T com)
{

    return 0;
}

void i2c_thread_task(void)
{
    static uint8_t led_flag = 0x00;
    //    i2c_write_data(I2C_COM0, I2C0_SLAVE_ADDRESS7, 0x06, 0xfe);
    i2c_write_data(I2C_COM0, I2C0_SLAVE_ADDRESS7, 0x02, led_flag);
    i2c_write_data(I2C_COM0, I2C0_SLAVE_1_ADDRESS7, 0x02, ~led_flag);
    //    i2c_read_reg(I2C_COM0, I2C0_SLAVE_ADDRESS7, 0x06);
    if (led_flag == 0)
        led_flag = 1;
    else
        led_flag = 0;
}

