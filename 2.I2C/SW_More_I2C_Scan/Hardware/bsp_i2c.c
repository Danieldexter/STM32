#include "bsp_i2c.h"

#define I2C_DelayUS(x)	Delay_US(x)
#define SDA_OUT_MODE(GPIOx,bit)	I2C_OUT_MODE(GPIOx,bit)
#define SCL_OUT_MODE(GPIOx,bit)	I2C_OUT_MODE(GPIOx,bit)
#define SDA_OUT_H(GPIOx,bit)	HAL_GPIO_WritePin(GPIOx,bit,GPIO_PIN_SET)		//SDA输出高电平
#define SDA_OUT_L(GPIOx,bit)	HAL_GPIO_WritePin(GPIOx,bit,GPIO_PIN_RESET)		//SDA输出低电平
#define SCL_OUT_H(GPIOx,bit)	HAL_GPIO_WritePin(GPIOx,bit,GPIO_PIN_SET)		//SCL输出高电平
#define SCL_OUT_L(GPIOx,bit)	HAL_GPIO_WritePin(GPIOx,bit,GPIO_PIN_RESET)		//SCL输出低电平
#define SDA_IN(GPIOx,bit)		HAL_GPIO_ReadPin(GPIOx,bit)					//SDA输入

void I2C_OUT_MODE(GPIO_TypeDef *I2C_GPIOx,uint8_t I2C_GPIO_BITx)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	switch((uint32_t)I2C_GPIOx)
	{
		case (u32)GPIOA:
			__HAL_RCC_GPIOA_CLK_ENABLE();
			break;
		case (u32)GPIOB:
			__HAL_RCC_GPIOB_CLK_ENABLE();
			break;
		case (u32)GPIOC:
			__HAL_RCC_GPIOC_CLK_ENABLE();
			break;
		case (u32)GPIOD:
			__HAL_RCC_GPIOD_CLK_ENABLE();
			break;
	}
	GPIO_InitStruct.Pin = I2C_GPIO_BITx;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(I2C_GPIOx, &GPIO_InitStruct);
}

void SDA_IN_MODE(GPIO_TypeDef *SDA_GPIOx,uint8_t SDA_GPIO_BITx)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	switch((uint32_t)SDA_GPIOx)
	{
		case (u32)GPIOA:
			__HAL_RCC_GPIOA_CLK_ENABLE();
			break;
		case (u32)GPIOB:
			__HAL_RCC_GPIOB_CLK_ENABLE();
			break;
		case (u32)GPIOC:
			__HAL_RCC_GPIOC_CLK_ENABLE();
			break;
		case (u32)GPIOD:
			__HAL_RCC_GPIOD_CLK_ENABLE();
			break;
	}
	GPIO_InitStruct.Pin = SDA_GPIO_BITx;
	GPIO_InitStruct.Pull=GPIO_NOPULL;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	HAL_GPIO_Init(SDA_GPIOx, &GPIO_InitStruct);
}

bool I2C_Init(I2C_HANDLE *pHandle,GPIO_TypeDef *SDA_GPIOx,GPIO_TypeDef *SCL_GPIOx,uint16_t SDA_GPIO_BITx, uint16_t SCL_GPIO_BITx,uint8_t DelayUS)
{
	if(pHandle == NULL)
	{
		printf("初始化软件IIC失败，pHandle句柄为空\r\n");
		return false;
	}
	if(DelayUS < 1) DelayUS = 1;
	if(DelayUS > 100) DelayUS = 100;
	
	pHandle->DelayUS = DelayUS;
	pHandle->SDA_GPIOx = SDA_GPIOx;				//SDA数据线IO
	pHandle->SCL_GPIOx = SCL_GPIOx;				//SCL时钟线IO
	
	pHandle->SDA_GPIO_BITx = SDA_GPIO_BITx;		//SDA数据线位，0-15
	pHandle->SCL_GPIO_BITx = SCL_GPIO_BITx;		//SCL时钟线位，0-15
	
	SDA_OUT_MODE(pHandle->SDA_GPIOx,pHandle->SDA_GPIO_BITx);		//SDA输出模式
	SCL_OUT_MODE(pHandle->SCL_GPIOx,pHandle->SCL_GPIO_BITx);		//SCL输出模式
	
	SDA_OUT_H(pHandle->SDA_GPIOx,pHandle->SDA_GPIO_BITx);//SDA=1
	SCL_OUT_H(pHandle->SCL_GPIOx,pHandle->SCL_GPIO_BITx);//SCL=1
	
	pHandle->Start = (void (*)(void *))I2C_Start;
	pHandle->Stop  = (void (*)(void *))I2C_Stop;
	pHandle->SendByte = (bool (*)(void *,uint8_t))I2C_SendByte;
	pHandle->ReadByte = (uint8_t (*)(void *,bool))I2C_ReadByte;
	return true;
}

/**
 * @brief I2C 开始,SCL为高电平的时候SDA产生一个下降沿信号
 * @retval none
 * @author halo
 * @date 2022-03-18
 */
void I2C_Start(I2C_HANDLE *pHandle)
{
	SDA_OUT_MODE(pHandle->SDA_GPIOx,pHandle->SDA_GPIO_BITx);
	SDA_OUT_H(pHandle->SDA_GPIOx,pHandle->SDA_GPIO_BITx);
	SCL_OUT_H(pHandle->SCL_GPIOx, pHandle->SCL_GPIO_BITx);		//SCL=1
	delay_us(4);								//延时
 	SDA_OUT_L(pHandle->SDA_GPIOx, pHandle->SDA_GPIO_BITx);		//SDA=0 START:when CLK is high,DATA change form high to low 
	delay_us(4);								//延时
	SCL_OUT_L(pHandle->SCL_GPIOx, pHandle->SCL_GPIO_BITx);		//SCL=0，钳住I2C总线，准备发送或接收数据 
}


void I2C_Stop(I2C_HANDLE *pHandle)
{
	SDA_OUT_MODE(pHandle->SDA_GPIOx,pHandle->SDA_GPIO_BITx);
	SCL_OUT_L(pHandle->SCL_GPIOx, pHandle->SCL_GPIO_BITx);		//SCL=0
	SDA_OUT_L(pHandle->SDA_GPIOx, pHandle->SDA_GPIO_BITx);		//SDA=0 //STOP:when CLK is high DATA change form low to high
	delay_us(4);								//延时
	SCL_OUT_H(pHandle->SCL_GPIOx, pHandle->SCL_GPIO_BITx);		//SCL=1					
	SDA_OUT_H(pHandle->SDA_GPIOx, pHandle->SDA_GPIO_BITx);		//SDA=1	
	delay_us(4);	//延时										//延时					   	
}


bool I2C_WaitAck(I2C_HANDLE *pHandle)
{
	uint8_t ucErrTime=0;
	SDA_IN_MODE(pHandle->SDA_GPIOx, pHandle->SDA_GPIO_BITx);	//SDA设置为输入
	SDA_OUT_H(pHandle->SDA_GPIOx, pHandle->SDA_GPIO_BITx);		//SDA=1	
	delay_us(1);								//延时
	SCL_OUT_H(pHandle->SCL_GPIOx, pHandle->SCL_GPIO_BITx);		//SCL=1
	delay_us(1);								//延时	 
	
	while(SDA_IN(pHandle->SDA_GPIOx, pHandle->SDA_GPIO_BITx))	//等待低电平
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			I2C_Stop(pHandle);
			return 1;
		}
	}	
	SCL_OUT_L(pHandle->SCL_GPIOx, pHandle->SCL_GPIO_BITx);		//SCL=0	  	
	return 0;  
} 

void I2C_Ack(I2C_HANDLE *pHandle)
{
	SCL_OUT_L(pHandle->SCL_GPIOx, pHandle->SCL_GPIO_BITx);		//SCL=0	   
	SDA_OUT_MODE(pHandle->SDA_GPIOx,pHandle->SDA_GPIO_BITx);
	SDA_OUT_L(pHandle->SDA_GPIOx, pHandle->SDA_GPIO_BITx);		//SDA=0
	delay_us(2);								//延时	 
	SCL_OUT_H(pHandle->SCL_GPIOx, pHandle->SCL_GPIO_BITx);		//SCL=1
	delay_us(2);								//延时	
	SCL_OUT_L(pHandle->SCL_GPIOx, pHandle->SCL_GPIO_BITx);		//SCL=0	 
}

void I2C_NAck(I2C_HANDLE *pHandle)
{
	SCL_OUT_L(pHandle->SCL_GPIOx, pHandle->SCL_GPIO_BITx);		//SCL=0	   
	SDA_OUT_MODE(pHandle->SDA_GPIOx,pHandle->SDA_GPIO_BITx);
	SDA_OUT_H(pHandle->SDA_GPIOx, pHandle->SDA_GPIO_BITx);		//SDA=1
	delay_us(2);								//延时	
	SCL_OUT_H(pHandle->SCL_GPIOx, pHandle->SCL_GPIO_BITx);		//SCL=1
	delay_us(2);								//延时	
	SCL_OUT_L(pHandle->SCL_GPIOx, pHandle->SCL_GPIO_BITx);		//SCL=0	  
}	


void I2C_SendByte(I2C_HANDLE *pHandle, uint8_t data)
{                        
    uint8_t t; 
	SDA_OUT_MODE(pHandle->SDA_GPIOx,pHandle->SDA_GPIO_BITx);
	SCL_OUT_L(pHandle->SCL_GPIOx, pHandle->SCL_GPIO_BITx);		//SCL=0	  
	for(t=0;t<8;t++)
    {         
		if(data & 0X80)
			SDA_OUT_H(pHandle->SDA_GPIOx, pHandle->SDA_GPIO_BITx);		//SDA=1
		else
			SDA_OUT_L(pHandle->SDA_GPIOx, pHandle->SDA_GPIO_BITx);		//SDA=0
        data <<= 1; 
		delay_us(2);								//延时	
		SCL_OUT_H(pHandle->SCL_GPIOx, pHandle->SCL_GPIO_BITx);		//SCL=1
		delay_us(2);								//延时	
		SCL_OUT_L(pHandle->SCL_GPIOx, pHandle->SCL_GPIO_BITx);		//SCL=0	
		delay_us(2);	
	}
} 

uint8_t I2C_ReadByte(I2C_HANDLE *pHandle,unsigned char isAck)
{
	unsigned char i,receive=0;
	
	SDA_IN_MODE(pHandle->SDA_GPIOx, pHandle->SDA_GPIO_BITx);		//SDA设置为输入
    for(i=0;i<8;i++ )
	{
		SCL_OUT_L(pHandle->SCL_GPIOx, pHandle->SCL_GPIO_BITx);		//SCL=0	
		delay_us(2);	
		SCL_OUT_H(pHandle->SCL_GPIOx, pHandle->SCL_GPIO_BITx);		//SCL=0	
		//延时	
		receive=receive<<1;
		if(SDA_IN(pHandle->SDA_GPIOx, pHandle->SDA_GPIO_BITx))
		{
			receive|=1;
		}
		delay_us(1);
    }
    if (!isAck)
        I2C_NAck(pHandle);//发送nACK
    else
        I2C_Ack(pHandle); //发送ACK  
    return receive;
}

//bool I2C_ReadReg(I2C_HANDLE *pHandle, u16 SlaveAddr, u16 RegAddr, bool is8bitRegAddr, u8 *pDataBuff, u16 ReadByteNum)
//{
//	u16 i;
//	
//	I2C_Start(pHandle);									//产生IIC起始信号
//	if(I2C_SendByte(pHandle, SlaveAddr) == false)			//发送设备地址+写信号
//	{
//		printf("[I2C ERROR]:地址(写)响应NACK\r\n");
//		return false;
//	}
//	if(is8bitRegAddr == false)								//16bit寄存器地址
//	{
//		if(I2C_SendByte(pHandle, RegAddr>>8) == false)		//发送寄存器地址高位
//		{
//			printf("[I2C ERROR]:寄存器地址MSB发送响应NACK\r\n");
//			return false;
//		}
//	}
//	if(I2C_SendByte(pHandle, RegAddr) == false)			//发送寄存器地址低位
//	{
//		printf("[I2C ERROR]:寄存器地址LSB发送响应NACK\r\n");
//		return false;
//	}
//	//发送读取
//	I2C_Start(pHandle);									//产生IIC起始信号
//	if(I2C_SendByte(pHandle, SlaveAddr|0x00) == false)		//发送设备地址+读信号
//	{
//		printf("[I2C ERROR]:地址(读)响应NACK\r\n");
//		return false;
//	}
//	//循环读取
//	for(i = 0;i < ReadByteNum;i ++)
//	{
//		if(i == (ReadByteNum-1))							//最后一字节不响应ACK
//		{
//			pDataBuff[i] = I2C_ReadByte(pHandle, false);	//SIIC读取一个字节-NAK
//		}
//		else
//		{
//			pDataBuff[i] = I2C_ReadByte(pHandle, true);	//SIIC读取一个字节-ACK
//		}	
//	}
//	I2C_Stop(pHandle);										//产生IIC停止信号
//	
//	return true;
//}

