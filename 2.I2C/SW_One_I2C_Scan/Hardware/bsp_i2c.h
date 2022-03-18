#ifndef __BSP_I2C_H
#define __BSP_I2C_H

#include "main.h"
#include <stdio.h>
#include <stdbool.h>

#define I2C_SDA_CLK()	__HAL_RCC_GPIOB_CLK_ENABLE()
#define I2C_SCL_CLK()	__HAL_RCC_GPIOB_CLK_ENABLE()
#define I2C_SDA_Port	GPIOB
#define I2C_SDA_Pin		GPIO_PIN_7
#define I2C_SCL_Port	GPIOB
#define I2C_SCL_Pin		GPIO_PIN_6
#define I2C_SDA_H		HAL_GPIO_WritePin(I2C_SDA_Port,I2C_SDA_Pin,GPIO_PIN_SET)
#define I2C_SDA_L		HAL_GPIO_WritePin(I2C_SDA_Port,I2C_SDA_Pin,GPIO_PIN_RESET)
#define I2C_SCL_H		HAL_GPIO_WritePin(I2C_SCL_Port,I2C_SCL_Pin,GPIO_PIN_SET)
#define I2C_SCL_L		HAL_GPIO_WritePin(I2C_SCL_Port,I2C_SCL_Pin,GPIO_PIN_RESET)
#define READ_SDA		HAL_GPIO_ReadPin(I2C_SDA_Port,I2C_SDA_Pin)

void I2C_Init(void);
void I2C_Start(void);
void I2C_Stop(void);
uint8_t I2C_Wait_Ack(void);
void I2C_Ack(void);
void I2C_NAck(void);
void I2C_Send_Byte(uint8_t txd);
uint8_t I2C_Read_Byte(unsigned char ack);

#endif
