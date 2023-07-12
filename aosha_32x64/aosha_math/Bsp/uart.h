#ifndef __UART_h
#define __UART_h

#include "gd32e10x.h"
#include "string.h"
#include "systick.h"
#include "stdbool.h"
#include "stdio.h"

typedef enum {
    USART_0_TR,
    USART_1_TR,
    USART_2_TR,
    USART_3_TR,
} USART_COM_ID_T;

void dma_test(void);

void usart0_init(uint32_t bps);
void usart1_init(uint32_t bps);
void usart3_init(uint32_t bps);
void usart2_init(uint32_t bps);

void usart_dma_send_data(USART_COM_ID_T com_id, uint8_t *data, uint32_t len);

int usart_rx_probe(USART_COM_ID_T com_id);
int usart_get_rx_data_count(USART_COM_ID_T com_id);
void usart_drop(USART_COM_ID_T com_id, uint8_t drop_lens);
unsigned char usart_at(USART_COM_ID_T com_id, uint8_t offset);
unsigned int usart_read(USART_COM_ID_T com_id,
                        uint8_t *data,
                        uint16_t len);

uint8_t usart_recv(USART_COM_ID_T com_id, uint8_t *data, uint16_t len);

#endif
