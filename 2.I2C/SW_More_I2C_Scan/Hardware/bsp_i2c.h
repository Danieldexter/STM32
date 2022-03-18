#ifndef __BSP_I2C_H
#define __BSP_I2C_H

#include "main.h"
#include <stdio.h>
#include <stdbool.h>

//���I2C�ӿھ���ṹ
typedef struct
{
	uint8_t DelayUS;
	GPIO_TypeDef *SDA_GPIOx;
	GPIO_TypeDef *SCL_GPIOx;
	uint16_t	SDA_GPIO_BITx;
	uint16_t SCL_GPIO_BITx;
	//�ӿں���
	void (*Start)(void *pHandle);				//����IIC��ʼ�ź�
	void (*Stop)(void *pHandle);				//����IICֹͣ�ź�
	bool (*SendByte)(void *pHandle, uint8_t data);	//SIIC����һ���ֽ�
	uint8_t (*ReadByte)(void *pHandle,bool isAck);	//SIIC��ȡһ���ֽ�
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
