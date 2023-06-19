#ifndef __CAN_h
#define __CAN_h

#include "gd32e10x.h"
#include "string.h"
#include "systick.h"
#include "stdbool.h"

void can0_init(void);
void read_can0_data(can_receive_message_struct *receive_message);
void send_can0_data(uint32_t id, uint8_t *msg);

#endif

