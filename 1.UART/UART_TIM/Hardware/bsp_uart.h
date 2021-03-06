#ifndef __BSP_UART_H
#define __BSP_UART_H

#include "main.h"
#include "usart.h"
#include "global.h"
#define BUF_LEN 100

extern uint32_t ulBaudRate ;

void usart1_byte_send(unsigned char *p, unsigned char len);
void usart1_str_send(unsigned char *p);
void usart1_send(unsigned char *p,unsigned char len);

typedef struct {                                     //????????????
    uint8_t rx_flag;
    uint8_t len;
    uint8_t rxmsg[BUF_LEN];    
}mdrxmsg;

extern mdrxmsg rxbuf[10];

#endif
