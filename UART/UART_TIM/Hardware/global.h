#ifndef __GLOBAL_H
#define __GLOBAL_H

#include "main.h"

extern uint8_t rxhead;
extern uint8_t rxtail;
extern uint8_t RevSta;

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t

#define LED0_ON 	HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_RESET)
#define LED0_OFF 	HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_SET)

#define LED1_ON		HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET)
#define LED1_OFF	HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET)

extern uint8_t u1rxdata;

















#endif
