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
unsigned int USART1_GetDataCount( void );
void USART1_Drop( unsigned short LenToDrop);
unsigned char USART1_At( unsigned short offset);
unsigned int USART1_Read(unsigned char *data, unsigned short len);
unsigned int USART1_Recv(unsigned char *data, unsigned short len);

#endif
