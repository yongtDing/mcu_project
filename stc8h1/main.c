/*---------------------------------------------------------------------*/
/* --- STC MCU Limited ------------------------------------------------*/
/* --- STC 1T Series MCU Demo Programme -------------------------------*/
/* --- Mobile: (86)13922805190 ----------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966 ------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/* --- BBS: www.STCAIMCU.com  -----------------------------------------*/
/* --- QQ:  800003751 -------------------------------------------------*/
/* 如果要在程序中使用此代码,请在程序中注明使用了STC的资料及程序            */
/*---------------------------------------------------------------------*/

#include	"config.h"
#include    "string.h"
#include	"STC8G_H_ADC.h"
#include	"STC8G_H_GPIO.h"
#include	"STC8G_H_Exti.h"
#include	"STC8G_H_UART.h"
#include	"STC8G_H_Delay.h"
#include	"STC8G_H_NVIC.h"
#include	"STC8G_H_Switch.h"
#include    "STC8G_H_EEPROM.h"

/*************	功能说明	**************

  本例程基于STC8H8K64U为主控芯片的实验箱8进行编写测试，STC8G、STC8H系列芯片可通用参考.

  演示INT0~INT4 5个唤醒源将MCU从休眠唤醒.

  从串口输出唤醒源跟唤醒次数，115200,N,8,1.

  下载时, 选择时钟 22.1184MHz (用户可在"config.h"修改频率).

 ******************************************/

/*************	本地常量声明	**************/


/*************	本地变量声明	**************/

u8 WakeUpCnt;

/*************	本地函数声明	**************/



/*************  外部函数和变量声明 *****************/



/******************** IO口配置 ********************/
void GPIO_config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;				//结构定义

    GPIO_InitStructure.Pin  = GPIO_Pin_3;			//指定要初始化的IO, GPIO_Pin_0 ~ GPIO_Pin_7, 或操作
    GPIO_InitStructure.Mode = GPIO_PullUp;			//指定IO的输入或输出方式,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
    GPIO_Inilize(GPIO_P3,&GPIO_InitStructure);	//初始化

	/* adc 10 IO */
	GPIO_InitStructure.Pin =  GPIO_Pin_2;
	GPIO_InitStructure.Mode = GPIO_HighZ;
	GPIO_Inilize(GPIO_P3,&GPIO_InitStructure);

    /* uart1 IO */
	GPIO_InitStructure.Pin  = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.Mode = GPIO_PullUp;
	GPIO_Inilize(GPIO_P3,&GPIO_InitStructure);
	
    /* LED O R */
	GPIO_InitStructure.Pin  = GPIO_Pin_7 | GPIO_Pin_6;
	GPIO_InitStructure.Mode = GPIO_OUT_OD;
	GPIO_Inilize(GPIO_P3,&GPIO_InitStructure);
	
	GPIO_InitStructure.Pin  = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.Mode = GPIO_OUT_OD;
	GPIO_Inilize(GPIO_P1,&GPIO_InitStructure);
	
	GPIO_InitStructure.Pin  = GPIO_Pin_4;
	GPIO_InitStructure.Mode = GPIO_OUT_OD;
	GPIO_Inilize(GPIO_P5,&GPIO_InitStructure);

    /* PLC STATE */
	GPIO_InitStructure.Pin  = GPIO_Pin_4;
	GPIO_InitStructure.Mode = GPIO_OUT_PP;
	GPIO_Inilize(GPIO_P3,&GPIO_InitStructure);

}

/******************* AD配置函数 *******************/
void	ADC_config(void)
{
	ADC_InitTypeDef		ADC_InitStructure;		//结构定义

	ADC_InitStructure.ADC_SMPduty   = 31;		//ADC 模拟信号采样时间控制, 0~31（注意： SMPDUTY 一定不能设置小于 10）
	ADC_InitStructure.ADC_CsSetup   = 0;		//ADC 通道选择时间控制 0(默认),1
	ADC_InitStructure.ADC_CsHold    = 3;		//ADC 通道选择保持时间控制 0,1(默认),2,3
	ADC_InitStructure.ADC_Speed     = ADC_SPEED_2X16T;		//设置 ADC 工作时钟频率	ADC_SPEED_2X1T~ADC_SPEED_2X16T
	ADC_InitStructure.ADC_AdjResult = ADC_RIGHT_JUSTIFIED;	//ADC结果调整,	ADC_LEFT_JUSTIFIED,ADC_RIGHT_JUSTIFIED
	ADC_Inilize(&ADC_InitStructure);		//初始化
	ADC_PowerControl(ENABLE);				//ADC电源开关, ENABLE或DISABLE
	NVIC_ADC_Init(DISABLE,Priority_0);		//中断使能, ENABLE/DISABLE; 优先级(低到高) Priority_0,Priority_1,Priority_2,Priority_3
}

/******************** INT配置 ********************/
void Exti_config(void)
{
    EXTI_InitTypeDef	Exti_InitStructure;							//结构定义

    Exti_InitStructure.EXTI_Mode      = EXT_MODE_Fall;//中断模式,   EXT_MODE_RiseFall,EXT_MODE_Fall
    Ext_Inilize(EXT_INT1,&Exti_InitStructure);				//初始化
    NVIC_INT1_Init(ENABLE,Priority_0);		//中断使能, ENABLE/DISABLE; 优先级(低到高) Priority_0,Priority_1,Priority_2,Priority_3
}

/****************  串口初始化函数 *****************/
void UART_config(void)
{
    COMx_InitDefine		COMx_InitStructure;				//结构定义

    COMx_InitStructure.UART_Mode      = UART_8bit_BRTx;	//模式, UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
    COMx_InitStructure.UART_BRT_Use   = BRT_Timer1;		//选择波特率发生器, BRT_Timer1, BRT_Timer2 (注意: 串口2固定使用BRT_Timer2)
    COMx_InitStructure.UART_BaudRate  = 115200ul;		//波特率, 一般 110 ~ 115200
    COMx_InitStructure.UART_RxEnable  = ENABLE;			//接收允许,   ENABLE或DISABLE
    COMx_InitStructure.BaudRateDouble = DISABLE;		//波特率加倍, ENABLE或DISABLE
    UART_Configuration(UART1, &COMx_InitStructure);		//初始化串口1 UART1,UART2,UART3,UART4
    NVIC_UART1_Init(ENABLE,Priority_1);		//中断使能, ENABLE/DISABLE; 优先级(低到高) Priority_0,Priority_1,Priority_2,Priority_3

    UART1_SW(UART1_SW_P30_P31);		//UART1_SW_P30_P31,UART1_SW_P36_P37,UART1_SW_P16_P17,UART1_SW_P43_P44
}

uint8 value_ladder[5] = {8, 33, 65, 90, 120};

#define PRINTF_BUFFER_SIZE 32
#define FLASH_ADDRESS_START 0x00

/******************** 主函数***********************/
void main(void)
{
    uint8 flag = 0;
    uint8 printf_buffer[PRINTF_BUFFER_SIZE] = {0x00};
    uint8 printf_buffer_len = 0;
    int16 plc_flag = 0;
    uint16 adc_value = 0;
    uint32 time_10 = 0;
    uint16 addr = 0x04;

    GPIO_config();
    ADC_config();
    UART_config();
    //	Exti_config();
    EA  = 1;		//Enable all interrupt


    P54 = 0;

#if 0
	EEPROM_SectorErase(addr);           //擦除扇区
	EEPROM_write_n(addr,&RX1_Buffer[9],j);      //写N个字节
	EEPROM_read_n(addr,tmp,j);
#endif

    PrintString1("system init finish\r\n");
    EEPROM_read_n(FLASH_ADDRESS_START, printf_buffer, 2);

    memcpy(&plc_flag, printf_buffer, 2);

    if (plc_flag < 3 || plc_flag > 1024)
        plc_flag = 3;


    sprintf(printf_buffer, "plc flag %d\r\n", plc_flag);
    PrintString1(printf_buffer);
    memset(printf_buffer, 0x00, PRINTF_BUFFER_SIZE);


    while(1)
    {
        time_10 ++;
        delay_ms(20);	//delay 10ms

        Exti_config();

        if(WakeUpSource == 2)
        {
            PrintString1("外中断INT1唤醒  \r\n");
            EEPROM_SectorErase(FLASH_ADDRESS_START);
            memset(printf_buffer, 0x00, PRINTF_BUFFER_SIZE);
            sprintf(printf_buffer, "write buffer %d\r\n", adc_value);
            PrintString1(printf_buffer);

            memset(printf_buffer, 0x00, PRINTF_BUFFER_SIZE);
            memcpy(printf_buffer, &adc_value, 2);
            plc_flag = adc_value;
            
            EEPROM_write_n(FLASH_ADDRESS_START, printf_buffer, 2);
        }
        WakeUpSource = 0;

        adc_value = Get_ADCResult(10);
        if (adc_value > 1024) adc_value = 1024;

#if 0
        if (adc_value > value_ladder[0])
            P36 = 0;
        else P36 = 1;

        if (adc_value > value_ladder[1])
            P37 = 0;
        else P37 = 1;

        if (adc_value > value_ladder[2])
            P15 = 0;
        else P15 = 1;

        if (adc_value > value_ladder[3])
            P16 = 0;
        else P16 = 1;

        if (adc_value > value_ladder[4])
            P17 = 0;
        else P17 = 1;
#else
        if (adc_value >= value_ladder[0] && adc_value < value_ladder[1])
            P36 = 0;
        else P36 = 1;

        if (adc_value >= value_ladder[1] && adc_value < value_ladder[2])
            P37 = 0;
        else P37 = 1;

        if (adc_value >= value_ladder[2] && adc_value < value_ladder[3])
            P15 = 0;
        else P15 = 1;

        if (adc_value >= value_ladder[3] && adc_value < value_ladder[4])
            P16 = 0;
        else P16 = 1;

        if (adc_value >= value_ladder[4])
            P17 = 0;
        else P17 = 1;

#endif

        if (adc_value > plc_flag)
        {
            P54 = 0;
            P34 = 0;
        }
        else
        {
            P54 = 1;
            P34 = 1;
        }

        flag = ~flag;
     
        if (!(time_10 % 20))
        {
            //sprintf(printf_buffer, "adc value %d\r\n", adc_value);
            if (printf_buffer_len != 0)
            {
                //memset(printf_buffer, 0x00, PRINTF_BUFFER_SIZE);
                //EEPROM_read_n(FLASH_ADDRESS_START, printf_buffer, printf_buffer_len);
                //PrintString1(printf_buffer);
            }
        }
    }
}

