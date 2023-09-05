#include "gd32e10x.h"
#include "string.h"
#include "systick.h"
#include "stdbool.h"
#include "uart.h"
#include "i2c.h"

/*----I2C1----SCL----PF1---*/
/*----I2C1----SDA----PF0---*/

/*----I2C1----SCL----PB6---*/
/*----I2C1----SDA----PB7---*/

                
#define IIC_SCL_H()      gpio_bit_set(GPIOB, GPIO_PIN_6)
#define IIC_SCL_L()      gpio_bit_reset(GPIOB, GPIO_PIN_6)
#define IIC_SDA_H()      gpio_bit_set(GPIOB, GPIO_PIN_7)
#define IIC_SDA_L()      gpio_bit_reset(GPIOB, GPIO_PIN_7)
#define IIC_SDA_Read()   gpio_input_bit_get(GPIOB, GPIO_PIN_7)


void IIC_Delay(unsigned int t)
{
	int i;
	for( i=0;i < t; i++)
	{
		int a = 10;//6
		while(a--);
	}
}


void IIC_GPIO_Init(void)
{
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_AF);
    /* configure LED2 GPIO port */
    //gpio_pin_remap_config(GPIO_SWJ_SWDPENABLE_REMAP, ENABLE);
	
    gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_6 | GPIO_PIN_7);
}

void IIC_SDA_Out(void)
{
    gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_7);
}

void IIC_SDA_In(void)
{
    gpio_init(GPIOB, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, GPIO_PIN_7);
}

void IIC_Start(void)								  
{
	IIC_SDA_Out();
	IIC_SDA_H();
	IIC_SCL_H();
	IIC_Delay(1);
	IIC_SDA_L();
	IIC_Delay(1);
	IIC_SCL_L();
}

void IIC_Stop(void)
{
	IIC_SDA_Out();
	IIC_SCL_L();
	IIC_SDA_L();
	IIC_Delay(1);
	IIC_SCL_H();
	IIC_SDA_H();
	IIC_Delay(1);
}

void IIC_Ack(u8 re)					     
{
	IIC_SDA_Out();
	if(re)
	   IIC_SDA_H();
	else
	   IIC_SDA_L();
	IIC_SCL_H();
	IIC_Delay(1);
	IIC_SCL_L();
	IIC_Delay(1);
}

int IIC_WaitAck(void)
{
	uint16_t Out_Time=1000;    
    IIC_SDA_H();
	IIC_SDA_In();
	IIC_Delay(1);
	IIC_SCL_H();
	IIC_Delay(1);
	while(IIC_SDA_Read())
	{
		if(--Out_Time)
		{
			IIC_Stop();
            //printf("error 2A\r\n");
            return 0xff;
		}
	}
	IIC_SCL_L();
    return 0;
}

void IIC_WriteBit(u8 Temp)
{
	u8 i;
	IIC_SDA_Out();
	IIC_SCL_L();
	for(i=0;i<8;i++)
	{
		if(Temp&0x80)
		{
			IIC_SDA_H();
		}
		else
		{
			IIC_SDA_L();
		}
		Temp<<=1;
		IIC_Delay(1);
		IIC_SCL_H();
		IIC_Delay(1);
		IIC_SCL_L();
	}
}

u8 IIC_ReadBit(void)
{
	u8 i,Temp=0;
	IIC_SDA_In();
	for(i=0;i<8;i++)
	{
		IIC_SCL_L();
		IIC_Delay(1);
		IIC_SCL_H();
		Temp<<=1;
		if(IIC_SDA_Read())
		   Temp++;
		IIC_Delay(1);
	}
	IIC_SCL_L();
	return Temp;
}

//写数据，成功返回0，失败返回0xff
int IIC_WriteData(u8 dev_addr,u8 reg_addr,u8 data)
{
	IIC_Start();
    
	IIC_WriteBit(dev_addr);
	if(IIC_WaitAck() == 0xff)
    {
        //printf("error 2B\r\n");
        return 0xff;
    }
    
	IIC_WriteBit(reg_addr);
	if(IIC_WaitAck() == 0xff)
    {
        //printf("error 2C\r\n");
        return 0xff;
    }

    IIC_WriteBit(data);
    if(IIC_WaitAck() == 0xff)
    {
        //printf("error 2D\r\n");
        return 0xff;
    }

	 IIC_Stop();
    return 0;
}

//读数据，成功返回0，失败返回0xff
int IIC_ReadData(u8 dev_addr,u8 reg_addr,u8 *pdata,u8 count)
{
	  u8 i;

    IIC_Start();
	
    IIC_WriteBit(dev_addr);
	  if(IIC_WaitAck() == 0xff)
    {
        //printf("error 2F\r\n");
        return 0xff;
    }
    
    IIC_WriteBit(reg_addr);
	  if(IIC_WaitAck() == 0xff)
    {
        //printf("error 2G\r\n");
        return 0xff;
    }
	
    IIC_Start();
    
    IIC_WriteBit(dev_addr+1);
	  if(IIC_WaitAck() == 0xff)
    {
        //printf("error 2H\r\n");
        return 0xff;
    }
    
    for(i=0;i<(count-1);i++)
    {
        *pdata=IIC_ReadBit();
        IIC_Ack(0);
        pdata++;
    }

    *pdata=IIC_ReadBit();
    IIC_Ack(1); 
    
    IIC_Stop(); 
    
    return 0;    
}

void WriteCmd(u8 command)
{
    IIC_Start();
    IIC_WriteBit(0x78);//OLED地址
    IIC_WaitAck();
    IIC_WriteBit(0x00);//寄存器地址
    IIC_WaitAck();
    IIC_WriteBit(command);
    IIC_WaitAck();
    IIC_Stop();
}


void WriteDat(u8 data)
{
    IIC_Start();
    IIC_WriteBit(0x78);//OLED地址
    IIC_WaitAck();
    IIC_WriteBit(0x40);//寄存器地址
    IIC_WaitAck();
    IIC_WriteBit(data);
    IIC_WaitAck();
    IIC_Stop();
}

