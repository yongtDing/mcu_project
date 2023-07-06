#include "uart.h"
#include "circular_buffer.h"

#define USART1_BUF_SIZE         10000
#define MAX_RING_BUF_SIZE       10000

unsigned char USART1_RxBuff[ MAX_RING_BUF_SIZE ] = {0};
unsigned char USART1_TxBuff[ MAX_RING_BUF_SIZE ] = {0};

CircBuf_t USART1_RxCBuf;

volatile uint8_t dma_idle = 1;

void nvic_config(void);
void dma0_ch4_usart0_rx_init(void);
void dma0_ch3_usart0_tx_init(void);
void dma_send_data(uint8_t *data, uint32_t len);
void gd_eval_com_init(uint32_t com);
void dma_test(void);

#define EVAL_COM0                        USART0
#define EVAL_COM0_CLK                    RCU_USART0
#define EVAL_COM0_TX_PIN                 GPIO_PIN_9
#define EVAL_COM0_RX_PIN                 GPIO_PIN_10
#define EVAL_COM0_GPIO_PORT              GPIOA
#define EVAL_COM0_GPIO_CLK               RCU_GPIOA
#define COMn                             1U


static rcu_periph_enum COM_CLK[COMn] = {EVAL_COM0_CLK};
static uint32_t COM_TX_PIN[COMn] = {EVAL_COM0_TX_PIN};
static uint32_t COM_RX_PIN[COMn] = {EVAL_COM0_RX_PIN};
static uint32_t COM_GPIO_PORT[COMn] = {EVAL_COM0_GPIO_PORT};
static rcu_periph_enum COM_GPIO_CLK[COMn] = {EVAL_COM0_GPIO_CLK};

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
    nvic_config();
    //dma init
    CircBuf_Init(&USART1_RxCBuf, USART1_RxBuff, MAX_RING_BUF_SIZE);

    dma0_ch3_usart0_tx_init();
    dma0_ch4_usart0_rx_init();
}


void dma0_ch4_usart0_rx_init(void)
{
    dma_parameter_struct dma_init_struct;

    /* enable DMA0 clock */
    rcu_periph_clock_enable(RCU_DMA0);
    /* initialize DMA0 channel4 */
    dma_deinit(DMA0, DMA_CH4);
    dma_init_struct.direction = DMA_PERIPHERAL_TO_MEMORY;
    dma_init_struct.memory_addr = (uint32_t)USART1_RxBuff;
    dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_init_struct.number = USART1_BUF_SIZE;
    dma_init_struct.periph_addr = (uint32_t)&USART_DATA(USART0);
    dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.memory_width = DMA_PERIPHERAL_WIDTH_8BIT;
    dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
    dma_init(DMA0, DMA_CH4, &dma_init_struct);

    /* configure DMA mode */
    dma_circulation_disable(DMA0, DMA_CH4);
    dma_memory_to_memory_disable(DMA0, DMA_CH4);

    /* USART DMA0 enable for reception */
    usart_dma_receive_config(USART0, USART_DENR_ENABLE);
    /* enable DMA0 channel4 transfer complete interrupt */
    dma_interrupt_enable(DMA0, DMA_CH4, DMA_INT_FTF);
    /* enable DMA0 channel4 */
    dma_channel_enable(DMA0, DMA_CH4);
    /* USART DMA0 enable for transmission */
    usart_dma_transmit_config(USART0, USART_DENT_ENABLE);
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
    dma_init_struct.memory_addr = (uint32_t)USART1_TxBuff;
    dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_init_struct.number = USART1_BUF_SIZE;
    dma_init_struct.periph_addr = (uint32_t)&USART_DATA(USART0);
    dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
    dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
    dma_init(DMA0, DMA_CH3, &dma_init_struct);

    /* configure DMA mode */
    dma_circulation_disable(DMA0, DMA_CH3);
    dma_memory_to_memory_disable(DMA0, DMA_CH3);
    /* enable DMA0 channel3 transfer complete interrupt */
    dma_interrupt_enable(DMA0, DMA_CH3, DMA_INT_FTF);

    /* enable DMA0 channel3 */
    //dma_channel_enable(DMA0, DMA_CH3);
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
    if(EVAL_COM0 == com)
    {
        com_id = 0U;
    }
    else
    {
        com_id = 2U;
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
    usart_baudrate_set(com, 460800);
    usart_receive_config(com, USART_RECEIVE_DISABLE);
    usart_transmit_config(com, USART_TRANSMIT_ENABLE);
    usart_enable(com);

    /* enable USART3 IDLEIE interrupt */
    usart_interrupt_enable(com, USART_INT_IDLE);
}



/*!
    \brief      configure DMA interrupt
    \param[in]  none
    \param[out] none
    \retval     none
*/
void nvic_config(void)
{
    nvic_irq_enable(DMA0_Channel3_IRQn, 0, 0);
    nvic_irq_enable(DMA0_Channel4_IRQn, 0, 1);
    nvic_irq_enable(USART0_IRQn, 1, 1);
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


void dma_send_data(uint8_t *data, uint32_t len)
{
    while(dma_idle == 0);
    dma_idle = 0;
    dma_channel_disable(DMA0, DMA_CH3);
    dma_memory_address_config(DMA0, DMA_CH3, (uint32_t)data);//设置要发送数据的内存地址
    dma_transfer_number_config(DMA0, DMA_CH3, len);//一共发多少个数据
    dma_channel_enable(DMA0, DMA_CH3);
    usart_dma_transmit_config(USART0, USART_DENT_ENABLE);//使能串口DMA发送
}

uint32_t get_dma_len = 0;

void USART0_IRQHandler(void)
{
    if(usart_interrupt_flag_get(USART0, USART_INT_FLAG_IDLE))
    {
        //清除空闲中断位，读取状态寄存器0（UASRT_STAT），再读取USART_DATA即可清除该标志位
        USART_STAT0(USART0);
        USART_DATA(USART0);
        //usart_data_receive(USART0);

		dma_channel_disable(DMA0, DMA_CH4);
        //得到已经接收的长度和重新配置DMA接收
        //get_len = 10 - DMA_CHCNT(DMA0, DMA_CH4);
        get_dma_len = USART1_BUF_SIZE - dma_transfer_number_get(DMA0, DMA_CH4);
        //把DMA数据存到环形缓冲
        CircBuf_Push(&USART1_RxCBuf, USART1_RxBuff, get_dma_len);

        dma_channel_disable(DMA0, DMA_CH4);
        //重新设置DMA接收数据的长度
        dma_transfer_number_config(DMA0, DMA_CH4, USART1_BUF_SIZE);
        dma_channel_enable(DMA0, DMA_CH4);
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

/**
 * @brief
 * @param[in]
 * @return
 */
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
unsigned int USART1_Read(unsigned char *data, unsigned short len)
{
    unsigned int result = 0;

    if(data != NULL)
        result = CircBuf_Read(&USART1_RxCBuf, data, len);

    return result;
}

/**
 * @brief
 * @param[in]
 * @return
 */
unsigned char USART1_At( unsigned short offset)
{
    return CircBuf_At(&USART1_RxCBuf, offset);
}


/**
 * @brief
 * @param[in]
 * @return
 */
void USART1_Drop( unsigned short LenToDrop)
{
    CircBuf_Drop(&USART1_RxCBuf, LenToDrop);
}


/**
 * @brief
 * @param[in]
 * @return
 */
unsigned int USART1_GetDataCount( void )
{
    return CircBuf_GetUsedSize(&USART1_RxCBuf);
}
