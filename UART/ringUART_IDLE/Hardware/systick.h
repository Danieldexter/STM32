#ifndef __SYSTICK_H
#define __SYSTICK_H

#include "main.h"

void delay_init(uint8_t SYSCLK);
void delay_us(uint32_t nus);
void delay_ms(uint16_t nms);

#endif
