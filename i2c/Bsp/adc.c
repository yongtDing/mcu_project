#include "adc.h"

volatile uint16_t adc_value[1][16];
float adc_v[8] = { 0 };

void read_adc_value(void)
{
    float sum = 0;

	for(int j = 0; j < 8; ++j)
    {
        sum = 0;
        for(int i = 0; i < 1; ++i)
        {
            sum += adc_value[i][j];
        }
        adc_v[j] = sum / 1 * 3.3f / 4096.0f;
	}
}

uint16_t GetAdcValue(uint8_t index)
{
    if(index < 16)
    {
        return adc_value[0][index];
    }
    else
    {
        return 0;
    }
}

void adc_init(void)
{
    /* system clocks configuration */
    rcu_config();
    /* systick configuration */
    systick_config();
    /* GPIO configuration */
    gpio_config();
    /* DMA configuration */
    dma_config();
    /* ADC configuration */
    adc_config();
}

/*!
    \brief      configure the different system clocks
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rcu_config(void)
{
    /* enable GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    /* enable ADC clock */
    rcu_periph_clock_enable(RCU_ADC0);
    /* enable DMA0 clock */
    rcu_periph_clock_enable(RCU_DMA0);
    /* config ADC clock */
    rcu_adc_clock_config(RCU_CKADC_CKAPB2_DIV4);
}

/*!
    \brief      configure the GPIO peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void gpio_config(void)
{
    /* config the GPIO as analog mode */
	gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_MAX, GPIO_PIN_0);
    gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_MAX, GPIO_PIN_1);
    gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_MAX, GPIO_PIN_2);
    gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_MAX, GPIO_PIN_3);
    gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_MAX, GPIO_PIN_4);
	gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_MAX, GPIO_PIN_5);
	gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_MAX, GPIO_PIN_6);
	gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_MAX, GPIO_PIN_7);

	gpio_init(GPIOB, GPIO_MODE_AIN, GPIO_OSPEED_MAX, GPIO_PIN_0);
	gpio_init(GPIOB, GPIO_MODE_AIN, GPIO_OSPEED_MAX, GPIO_PIN_1);

    gpio_init(GPIOC, GPIO_MODE_AIN, GPIO_OSPEED_MAX, GPIO_PIN_0);
    gpio_init(GPIOC, GPIO_MODE_AIN, GPIO_OSPEED_MAX, GPIO_PIN_1);
    gpio_init(GPIOC, GPIO_MODE_AIN, GPIO_OSPEED_MAX, GPIO_PIN_2);
    gpio_init(GPIOC, GPIO_MODE_AIN, GPIO_OSPEED_MAX, GPIO_PIN_3);
    gpio_init(GPIOC, GPIO_MODE_AIN, GPIO_OSPEED_MAX, GPIO_PIN_4);
	gpio_init(GPIOC, GPIO_MODE_AIN, GPIO_OSPEED_MAX, GPIO_PIN_5);

}

/*!
    \brief      configure the DMA peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dma_config(void)
{
    /* ADC_DMA_channel configuration */
    dma_parameter_struct dma_data_parameter;

    /* ADC DMA_channel configuration */
    dma_deinit(DMA0, DMA_CH0);

    /* initialize DMA single data mode */
    dma_data_parameter.periph_addr = (uint32_t)(&ADC_RDATA(ADC0));
    dma_data_parameter.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_data_parameter.memory_addr = (uint32_t)(&adc_value);
    dma_data_parameter.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_data_parameter.periph_width = DMA_PERIPHERAL_WIDTH_16BIT;
    dma_data_parameter.memory_width = DMA_MEMORY_WIDTH_16BIT;
    dma_data_parameter.direction = DMA_PERIPHERAL_TO_MEMORY;
    dma_data_parameter.number = 16;
    dma_data_parameter.priority = DMA_PRIORITY_HIGH;
    dma_init(DMA0, DMA_CH0, &dma_data_parameter);

    dma_circulation_enable(DMA0, DMA_CH0);

    /* enable DMA channel */
    dma_channel_enable(DMA0, DMA_CH0);
    //dma interrupt
    //dma_interrupt_enable(DMA0, DMA_CH0, DMA_INT_FTF);
}

void DMA0_Channel0_IRQHandler(void)
{
    if(dma_interrupt_flag_get(DMA0, DMA_CH0, DMA_INT_FLAG_FTF))
    {
        dma_interrupt_flag_clear(DMA0, DMA_CH0, DMA_INT_FLAG_G);
        read_adc_value();
    }
}


/*!
    \brief      configure the ADC peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void adc_config(void)
{
    /* reset ADC */
    adc_deinit(ADC0);
    /* ADC mode config */
    adc_mode_config(ADC_MODE_FREE);
    /* ADC contineous function enable */
    adc_special_function_config(ADC0, ADC_CONTINUOUS_MODE, ENABLE);
    /* ADC scan mode disable */
    adc_special_function_config(ADC0, ADC_SCAN_MODE, ENABLE);
    /* ADC data alignment config */
    adc_data_alignment_config(ADC0, ADC_DATAALIGN_RIGHT);

    /* ADC channel length config */
    adc_channel_length_config(ADC0, ADC_REGULAR_CHANNEL, 16);
    /* ADC regular channel config */
    adc_regular_channel_config(ADC0, 0, ADC_CHANNEL_0, ADC_SAMPLETIME_55POINT5);
    adc_regular_channel_config(ADC0, 1, ADC_CHANNEL_1, ADC_SAMPLETIME_55POINT5);
    adc_regular_channel_config(ADC0, 2, ADC_CHANNEL_2, ADC_SAMPLETIME_55POINT5);
    adc_regular_channel_config(ADC0, 3, ADC_CHANNEL_3, ADC_SAMPLETIME_55POINT5);
    adc_regular_channel_config(ADC0, 4, ADC_CHANNEL_4, ADC_SAMPLETIME_55POINT5);
    adc_regular_channel_config(ADC0, 5, ADC_CHANNEL_5, ADC_SAMPLETIME_55POINT5);
    adc_regular_channel_config(ADC0, 6, ADC_CHANNEL_6, ADC_SAMPLETIME_55POINT5);
    adc_regular_channel_config(ADC0, 7, ADC_CHANNEL_7, ADC_SAMPLETIME_55POINT5);
    adc_regular_channel_config(ADC0, 8, ADC_CHANNEL_8, ADC_SAMPLETIME_55POINT5);
    adc_regular_channel_config(ADC0, 9, ADC_CHANNEL_9, ADC_SAMPLETIME_55POINT5);
    adc_regular_channel_config(ADC0, 10, ADC_CHANNEL_10, ADC_SAMPLETIME_55POINT5);
    adc_regular_channel_config(ADC0, 11, ADC_CHANNEL_11, ADC_SAMPLETIME_55POINT5);
    adc_regular_channel_config(ADC0, 12, ADC_CHANNEL_12, ADC_SAMPLETIME_55POINT5);
    adc_regular_channel_config(ADC0, 13, ADC_CHANNEL_13, ADC_SAMPLETIME_55POINT5);
    adc_regular_channel_config(ADC0, 14, ADC_CHANNEL_14, ADC_SAMPLETIME_55POINT5);
    adc_regular_channel_config(ADC0, 15, ADC_CHANNEL_15, ADC_SAMPLETIME_55POINT5);


    /* ADC trigger config *///规则通道组， 软件触发
    adc_external_trigger_source_config(ADC0, ADC_REGULAR_CHANNEL, ADC0_1_EXTTRIG_REGULAR_NONE);
    adc_external_trigger_config(ADC0, ADC_REGULAR_CHANNEL, ENABLE);

    /* enable ADC interface */
    adc_enable(ADC0);
    delay_1ms(100);
    /* ADC calibration and reset calibration */
    adc_calibration_enable(ADC0);

    /* ADC DMA function enable */
    adc_dma_mode_enable(ADC0);

    /* ADC software trigger enable */
    adc_software_trigger_enable(ADC0, ADC_REGULAR_CHANNEL);
}
