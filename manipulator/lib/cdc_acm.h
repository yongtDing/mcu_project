#ifndef __CDC_ACM_H__
#define __CDC_ACM_H__

#include "stdint.h"

extern void usb_init(void **usb_handle);
extern void TIMER2_IRQHandler_lib(void);
extern void  USBFS_IRQHandler_lib(void);
extern void USBFS_WKUP_IRQHandler_lib(void);
extern void usb_send_buffer(void *cdc_acm, uint8_t *respond, int len);
extern void cdc_usb_loop(void *usb_handle);
#endif
