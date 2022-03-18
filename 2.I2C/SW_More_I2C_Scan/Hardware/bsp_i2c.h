#ifndef __BSP_I2C_H
#define __BSP_I2C_H

#include "main.h"
#include <stdio.h>
#include <stdbool.h>

//软件I2C接口句柄结构
typedef struct
{
	uint8_t DelayUS;
	GPIO_TypeDef *SDA_GPIOx;
	GPIO_TypeDef *SCL_GPIOx;
	uint16_t	SDA_GPIO_BITx;
	uint16_t SCL_GPIO_BITx;
	//接口函数
	void (*Start)(void *pHandle);				//产生IIC起始信号
	void (*Stop)(void *pHandle);				//产生IIC停止信号
	bool (*SendByte)(void *pHandle, uint8_t data);	//SIIC发送一个字节
	uint8_t (*ReadByte)(void *pHandle,bool isAck);	//SIIC读取一个字节
}I2C_HANDLE;

bool I2C_Init(I2C_HANDLE *pHandle,GPIO_TypeDef *SDA_GPIOx,GPIO_TypeDef *SCL_GPIOx,uint16_t SDA_GPIO_BITx, uint16_t SCL_GPIO_BITx,uint8_t DelayUS);
void I2C_Start(I2C_HANDLE *pHandle);
void I2C_Stop(I2C_HANDLE *pHandle);
bool I2C_WaitAck(I2C_HANDLE *pHandle);
void I2C_Ack(I2C_HANDLE *pHandle);
void I2C_NAck(I2C_HANDLE *pHandle);
void I2C_SendByte(I2C_HANDLE *pHandle, uint8_t data);
uint8_t I2C_ReadByte(I2C_HANDLE *pHandle,unsigned char isAck);
bool I2C_ReadReg(I2C_HANDLE *pHandle, u16 SlaveAddr, u16 RegAddr, bool is8bitRegAddr, u8 *pDataBuff, u16 ReadByteNum);
















#endif
