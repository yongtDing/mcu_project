#include "uart.h"
#include "circular_buffer.h"
#include "led.h"

#define USART0_BUF_SIZE         512
#define MAX_RING_BUF_SIZE       512
#define USART_RX_CACHE_BUFFER_SIZE 128

unsigned char USART0_RxBuff[MAX_RING_BUF_SIZE] = {0};
unsigned char USART0_TxBuff[MAX_RING_BUF_SIZE] = {0};

unsigned char USART1_RxBuff[MAX_RING_BUF_SIZE] = {0};
unsigned char USART1_TxBuff[MAX_RING_BUF_SIZE] = {0};

uint8_t USART0_RX_CACHE_BUFFER[USART_RX_CACHE_BUFFER_SIZE] = {0};
uint8_t USART1_RX_CACHE_BUFFER[USART_RX_CACHE_BUFFER_SIZE] = {0};

CircBuf_t USART0_RxCBuf = {0};
CircBuf_t USART1_RxCBuf = {0};

volatile uint8_t dma_idle = 1;
volatile uint8_t dma_ch6_idle = 1;

void nvic_config(uint32_t com);
void dma0_ch4_usart0_rx_init(void);
void dma0_ch3_usart0_tx_init(void);
void dma0_ch5_usart1_rx_init(void);
void dma0_ch6_usart1_tx_init(void);
void gd_eval_com_init(uint32_t com);
void dma_test(void);

#define EVAL_COM0                        USART0
#define EVAL_COM0_CLK                    RCU_USART0
#define EVAL_COM0_TX_PIN                 GPIO_PIN_9
#define EVAL_COM0_RX_PIN                 GPIO_PIN_10
#define EVAL_COM0_GPIO_PORT              GPIOA
#define EVAL_COM0_GPIO_CLK               RCU_GPIOA

#define EVAL_COM1                        USART1
#define EVAL_COM1_CLK                    RCU_USART1
#define EVAL_COM1_TX_PIN                 GPIO_PIN_2
#define EVAL_COM1_RX_PIN                 GPIO_PIN_3
#define EVAL_COM1_GPIO_PORT              GPIOA
#define EVAL_COM1_GPIO_CLK               RCU_GPIOA
#define COMn                             2U


static rcu_periph_enum COM_CLK[COMn] = {EVAL_COM0_CLK, EVAL_COM1_CLK};
static uint32_t COM_TX_PIN[COMn] = {EVAL_COM0_TX_PIN, EVAL_COM1_TX_PIN};
static uint32_t COM_RX_PIN[COMn] = {EVAL_COM0_RX_PIN, EVAL_COM1_RX_PIN};
static uint32_t COM_GPIO_PORT[COMn] = {EVAL_COM0_GPIO_PORT, EVAL_COM1_GPIO_PORT};
static rcu_periph_enum COM_GPIO_CLK[COMn] = {EVAL_COM0_GPIO_CLK, EVAL_COM1_GPIO_CLK};

/*!
  \brief      main function
  \param[in]  none
  \param[out] none
  \retval     none
  */
void usart0_init(void)
{
    /*usart0 init*/
    gd_eval_com_init(EVAL_COM0);
    /*configure DMA0 and usart idle interrupt*/
    nvic_config(EVAL_COM0);
    //dma init
    CircBuf_Init(&USART0_RxCBuf, USART0_RxBuff, MAX_RING_BUF_SIZE);

    dma0_ch3_usart0_tx_init();
    dma0_ch4_usart0_rx_init();
}

void usart1_init(void)
{
    gd_eval_com_init(EVAL_COM1);
    nvic_config(EVAL_COM1);
    CircBuf_Init(&USART1_RxCBuf, USART1_RxBuff, MAX_RING_BUF_SIZE);

    dma0_ch6_usart1_tx_init();
    dma0_ch5_usart1_rx_init();
}

void dma0_ch4_usart0_rx_init(void)
{
    dma_parameter_struct dma_init_struct;

    /* enable DMA0 clock */
    rcu_periph_clock_enable(RCU_DMA0);
    /* initialize DMA0 channel4 */
    dma_deinit(DMA0, DMA_CH4);
    dma_struct_para_init(&dma_init_struct);

    dma_init_struct.direction = DMA_PERIPHERAL_TO_MEMORY;
    dma_init_struct.memory_addr = (uint32_t)USART0_RX_CACHE_BUFFER;
    dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_init_struct.number = USART_RX_CACHE_BUFFER_SIZE;
    dma_init_struct.periph_addr = (uint32_t)&USART_DATA(USART0);
    dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.memory_width = DMA_PERIPHERAL_WIDTH_8BIT;
    dma_init_struct.priority = DMA_PRIORITY_LOW;
    dma_init(DMA0, DMA_CH4, &dma_init_struct);

    /* configure DMA mode */
    dma_circulation_disable(DMA0, DMA_CH4);
    dma_memory_to_memory_disable(DMA0, DMA_CH4);

    /* USART DMA0 enable for reception */
    usart_dma_receive_config(USART0, USART_DENR_ENABLE);

    /* enable DMA0 channel4 transfer complete interrupt */
//    dma_interrupt_enable(DMA0, DMA_CH4, DMA_INT_FTF);

    /* enable DMA0 channel4 */
    dma_channel_enable(DMA0, DMA_CH4);

    /* USART DMA0 enable for transmission */
//    usart_dma_transmit_config(USART0, USART_DENT_ENABLE);
}

void dma0_ch5_usart1_rx_init(void)
{
    dma_parameter_struct dma_init_struct = {0};

    /* enable DMA0 clock */
    rcu_periph_clock_enable(RCU_DMA0);
    /* initialize DMA0 channel5 */
    dma_deinit(DMA0, DMA_CH5);
    dma_struct_para_init(&dma_init_struct);

    dma_init_struct.direction = DMA_PERIPHERAL_TO_MEMORY;
    dma_init_struct.memory_addr = (uint32_t)USART1_RX_CACHE_BUFFER;
    dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_init_struct.number = USART_RX_CACHE_BUFFER_SIZE;
    dma_init_struct.periph_addr = (uint32_t)&USART_DATA(USART1);
    dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.memory_width = DMA_PERIPHERAL_WIDTH_8BIT;
    dma_init_struct.priority = DMA_PRIORITY_LOW;
    dma_init(DMA0, DMA_CH5, &dma_init_struct);

    /* configure DMA mode */
    dma_circulation_disable(DMA0, DMA_CH5);
    dma_memory_to_memory_disable(DMA0, DMA_CH5);

    /* USART DMA0 enable for reception */
    usart_dma_receive_config(USART1, USART_DENR_ENABLE);

    /* enable DMA0 channel4 transfer complete interrupt */
//    dma_interrupt_enable(DMA0, DMA_CH5, DMA_INT_FTF);

    /* enable DMA0 channel4 */
    dma_channel_enable(DMA0, DMA_CH5);

    /* USART DMA0 enable for transmission */
//    usart_dma_transmit_config(USART0, USART_DENT_ENABLE);
}


void dma0_ch3_usart0_tx_init(void)
{
    dma_parameter_struct dma_init_struct;

    /* enable DMA0 clock */
    rcu_periph_clock_enable(RCU_DMA0);

    /* initialize DMA0 channel3 */
    dma_deinit(DMA0, DMA_CH3);
    dma_struct_para_init(&dma_init_struct);

    dma_init_struct.direction = DMA_MEMORY_TO_PERIPHERAL;
    dma_init_struct.memory_addr = (uint32_t)USART0_TxBuff;
    dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_init_struct.number = MAX_RING_BUF_SIZE;
    dma_init_struct.periph_addr = (uint32_t)&USART_DATA(USART0);
    dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
    dma_init_struct.priority = DMA_PRIORITY_LOW;
    dma_init(DMA0, DMA_CH3, &dma_init_struct);

    /* configure DMA mode */
    dma_circulation_disable(DMA0, DMA_CH3);
    dma_memory_to_memory_disable(DMA0, DMA_CH3);

    /* enable DMA0 channel3 transfer complete interrupt */
    dma_interrupt_enable(DMA0, DMA_CH3, DMA_INT_FTF);

    /* enable DMA0 channel3 */
    dma_channel_disable(DMA0, DMA_CH3);
}

void dma0_ch6_usart1_tx_init(void)
{
    dma_parameter_struct dma_init_struct;

    /* enable DMA0 clock */
    rcu_periph_clock_enable(RCU_DMA0);

    /* initialize DMA0 channel3 */
    dma_deinit(DMA0, DMA_CH6);
    dma_struct_para_init(&dma_init_struct);

    dma_init_struct.direction = DMA_MEMORY_TO_PERIPHERAL;
    dma_init_struct.memory_addr = (uint32_t)USART1_TxBuff;
    dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_init_struct.number = MAX_RING_BUF_SIZE;
    dma_init_struct.periph_addr = (uint32_t)&USART_DATA(USART1);
    dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
    dma_init_struct.priority = DMA_PRIORITY_LOW;
    dma_init(DMA0, DMA_CH6, &dma_init_struct);

    /* configure DMA mode */
    dma_circulation_disable(DMA0, DMA_CH6);
    dma_memory_to_memory_disable(DMA0, DMA_CH6);

    /* enable DMA0 channel3 transfer complete interrupt */
    dma_interrupt_enable(DMA0, DMA_CH6, DMA_INT_FTF);

    /* enable DMA0 channel3 */
    dma_channel_disable(DMA0, DMA_CH6);
}



/*!
  \brief      configure COM port
  \param[in]  com: COM on the board
  \arg        EVAL_COM0: COM0 on the board
  \arg        EVAL_COM1: COM1 on the board
  \param[out] none
  \retval     none
  */
void gd_eval_com_init(uint32_t com)
{
    uint32_t com_id = 0U;
    if(EVAL_COM0 == com) {
        com_id = 0U;
    } else if (EVAL_COM1 == com) {
        com_id = 1U;
    }

    /* enable GPIO clock */
    rcu_periph_clock_enable(COM_GPIO_CLK[com_id]);

    /* enable USART clock */
    rcu_periph_clock_enable(COM_CLK[com_id]);

    /* connect port to USARTx_Tx */
    gpio_init(COM_GPIO_PORT[com_id], GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, COM_TX_PIN[com_id]);

    /* connect port to USARTx_Rx */
    gpio_init(COM_GPIO_PORT[com_id], GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, COM_RX_PIN[com_id]);

    /* USART configure */
    usart_deinit(com);
    usart_baudrate_set(com, 115200U);
    usart_receive_config(com, USART_RECEIVE_ENABLE);
    usart_transmit_config(com, USART_TRANSMIT_ENABLE);
    usart_enable(com);

    /* enable USART IDLEIE interrupt */
    usart_interrupt_enable(com, USART_INT_IDLE);
}

/*!
  \brief      configure DMA interrupt
  \param[in]  none
  \param[out] none
  \retval     none
  */
void nvic_config(uint32_t com)
{
    if (com == EVAL_COM0)
    {
        nvic_irq_enable(DMA0_Channel3_IRQn, 0, 1); //TX
        nvic_irq_enable(DMA0_Channel4_IRQn, 0, 0); //RX
        nvic_irq_enable(USART0_IRQn, 1, 1);
    } else if (com == EVAL_COM1) {
        nvic_irq_enable(DMA0_Channel5_IRQn, 0, 0); //RX
        nvic_irq_enable(DMA0_Channel6_IRQn, 0, 1);  //TX
        nvic_irq_enable(USART1_IRQn, 1, 1);
    }
}


/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(EVAL_COM0, (uint8_t)ch);
    while(RESET == usart_flag_get(EVAL_COM0, USART_FLAG_TBE));
    return ch;
}

/*!
  \brief      this function handles DMA0_Channel3_IRQHandler interrupt tx
  \param[in]  none
  \param[out] none
  \retval     none
  */
void DMA0_Channel3_IRQHandler(void)
{
    if(dma_interrupt_flag_get(DMA0, DMA_CH3, DMA_INT_FLAG_FTF))
    {
        dma_interrupt_flag_clear(DMA0, DMA_CH3, DMA_INT_FLAG_G);
        dma_idle = 1;
    }
}

void DMA0_Channel6_IRQHandler(void)
{
    if(dma_interrupt_flag_get(DMA0, DMA_CH6, DMA_INT_FLAG_FTF))
    {
        dma_interrupt_flag_clear(DMA0, DMA_CH6, DMA_INT_FLAG_G);
        dma_ch6_idle = 1;
    }
}

void usart0_dma_send_data(uint8_t *data, uint32_t len)
{
    while(dma_idle == 0);
    dma_idle = 0;
    dma_channel_disable(DMA0, DMA_CH3);
    dma_memory_address_config(DMA0, DMA_CH3, (uint32_t)data);//设置要发送数据的内存地址
    dma_transfer_number_config(DMA0, DMA_CH3, len);//一共发多少个数据
    dma_channel_enable(DMA0, DMA_CH3);
    usart_dma_transmit_config(USART0, USART_DENT_ENABLE);//使能串口DMA发送
}

void usart1_dma_send_data(uint8_t *data, uint32_t len)
{
    while(dma_ch6_idle == 0);
    dma_ch6_idle = 0;
    dma_channel_disable(DMA0, DMA_CH6);
    dma_memory_address_config(DMA0, DMA_CH6, (uint32_t)data);//设置要发送数据的内存地址
    dma_transfer_number_config(DMA0, DMA_CH6, len);//一共发多少个数据
    dma_channel_enable(DMA0, DMA_CH6);
    usart_dma_transmit_config(USART1, USART_DENT_ENABLE);//使能串口DMA发送
}

void USART0_IRQHandler(void)
{
    uint32_t get_dma_len = 0;
    if(usart_interrupt_flag_get(USART0, USART_INT_FLAG_IDLE))
    {
        //清除空闲中断位，读取状态寄存器0（UASRT_STAT），再读取USART_DATA即可清除该标志位
        USART_STAT0(USART0);
        USART_DATA(USART0);
//        usart_data_receive(USART0);

        dma_channel_disable(DMA0, DMA_CH4);
        //得到已经接收的长度和重新配置DMA接收
        //get_len = 10 - DMA_CHCNT(DMA0, DMA_CH4);
        get_dma_len = USART_RX_CACHE_BUFFER_SIZE - dma_transfer_number_get(DMA0, DMA_CH4);
        //把DMA数据存到环形缓冲
        CircBuf_Push(&USART0_RxCBuf, USART0_RX_CACHE_BUFFER, get_dma_len);

        dma_channel_disable(DMA0, DMA_CH4);
        //重新设置DMA接收数据的长度
        dma_transfer_number_config(DMA0, DMA_CH4, USART_RX_CACHE_BUFFER_SIZE);
        dma_channel_enable(DMA0, DMA_CH4);
    }
}


void USART1_IRQHandler(void)
{
    uint32_t get_dma_len = 0;

    if(usart_interrupt_flag_get(USART1, USART_INT_FLAG_IDLE))
    {
        //清除空闲中断位，读取状态寄存器0（UASRT_STAT），再读取USART_DATA即可清除该标志位
        USART_STAT0(USART1);
        USART_DATA(USART1);
//        usart_data_receive(USART1);

        dma_channel_disable(DMA0, DMA_CH5);
        //得到已经接收的长度和重新配置DMA接收
        //get_len = 10 - DMA_CHCNT(DMA0, DMA_CH5);
        get_dma_len = USART_RX_CACHE_BUFFER_SIZE - dma_transfer_number_get(DMA0, DMA_CH5);
        //把DMA数据存到环形缓冲
        CircBuf_Push(&USART1_RxCBuf, USART1_RX_CACHE_BUFFER, get_dma_len);

        dma_channel_disable(DMA0, DMA_CH5);
        //重新设置DMA接收数据的长度
        dma_transfer_number_config(DMA0, DMA_CH5, USART_RX_CACHE_BUFFER_SIZE);
        dma_channel_enable(DMA0, DMA_CH5);
    }
}

/*!
  \brief      this function handles DMA0_Channel4_IRQHandler interrupt rx
  \param[in]  none
  \param[out] none
  \retval     none
  */
void DMA0_Channel4_IRQHandler(void)
{
    if(dma_interrupt_flag_get(DMA0, DMA_CH4, DMA_INT_FLAG_FTF))
    {
        dma_interrupt_flag_clear(DMA0, DMA_CH4, DMA_INT_FLAG_G);
    }
}

uint8_t usart0_send_buffer(uint8_t *data, uint16_t len)
{
    CircBuf_Push(&USART0_RxCBuf, data, len);
    return 0;
}

uint8_t usart1_send_buffer(uint8_t *data, uint16_t len)
{
    CircBuf_Push(&USART1_RxCBuf, data, len);
    return 0;
}


/**
 * @brief
 * @param[in]
 * @return
 */
unsigned int USART0_Recv(unsigned char *data, unsigned short len)
{
    unsigned int result = 0;

    if(data != NULL)
        result = CircBuf_Pop(&USART0_RxCBuf, data, len);

    return result;
}

unsigned int USART1_Recv(unsigned char *data, unsigned short len)
{
    unsigned int result = 0;

    if(data != NULL)
        result = CircBuf_Pop(&USART1_RxCBuf, data, len);

    return result;
}

/**
 * @brief
 * @param[in]
 * @return
 */
unsigned int USART0_Read(unsigned char *data, unsigned short len)
{
    unsigned int result = 0;

    if(data != NULL)
        result = CircBuf_Read(&USART0_RxCBuf, data, len);

    return result;
}

/**
 * @brief
 * @param[in]
 * @return
 */
unsigned char USART0_At( unsigned short offset)
{
    return CircBuf_At(&USART0_RxCBuf, offset);
}


/**
 * @brief
 * @param[in]
 * @return
 */
void USART0_Drop( unsigned short LenToDrop)
{
    CircBuf_Drop(&USART0_RxCBuf, LenToDrop);
}


/**
 * @brief
 * @param[in]
 * @return
 */
unsigned int USART0_GetDataCount( void )
{
    return CircBuf_GetUsedSize(&USART0_RxCBuf);
}

unsigned int USART1_GetDataCount(void)
{
    return CircBuf_GetUsedSize(&USART1_RxCBuf);
}

int usart0_rx_probe(void)
{
    return CircBuf_IsEmpty(&USART0_RxCBuf);
}

int usart1_rx_probe(void)
{
    return CircBuf_IsEmpty(&USART1_RxCBuf);
}

