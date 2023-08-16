#ifndef __UART_h
#define __UART_h

#include "gd32e10x.h"
#include "string.h"
#include "systick.h"
#include "stdbool.h"
#include "stdio.h"

void dma_test(void);
void usart0_init(void);
void dma_send_data(uint8_t *data, uint32_t len);
void dma_send_data(uint8_t *data, uint32_t len);
unsigned int USART0_GetDataCount( void );
void USART0_Drop( unsigned short LenToDrop);
unsigned char USART0_At( unsigned short offset);
unsigned int USART0_Read(unsigned char *data, unsigned short len);
unsigned int USART0_Recv(unsigned char *data, unsigned short len);
uint8_t usart_send_buffer(uint8_t *data, uint16_t len);
int usart0_rx_probe(void);

void usart1_init(void);
unsigned int USART1_Recv(unsigned char *data, unsigned short len);
int usart1_rx_probe(void);
unsigned int USART1_GetDataCount(void);

void usart0_dma_send_data(uint8_t *data, uint32_t len);
void usart1_dma_send_data(uint8_t *data, uint32_t len);

uint8_t usart0_send_buffer(uint8_t *data, uint16_t len);
uint8_t usart1_send_buffer(uint8_t *data, uint16_t len);
#endif
