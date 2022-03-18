#include "24cxx.h"

/**
 * @brief AT24CXX IO 初始化
 * @retval none
 * @author halo
 * @date 2022-03-18
 */
void AT24CXX_Init(void)
{
	I2C_Init();
}

/**
 * @brief 在AT24CXX指定地址读出一个数据
 *		  ReadAddr:开始读数的地址  
 * @retval temp 读到得数据
 * @author halo
 * @date 2022-03-18
 */
uint8_t AT24CXX_ReadOneByte(uint16_t ReadAddr)
{				  
	uint8_t temp=0;		  	    																 
    I2C_Start();  
	if(EE_TYPE>AT24C16)
	{
		I2C_Send_Byte(0XA0);	   //发送写命令
		I2C_Wait_Ack();
		I2C_Send_Byte(ReadAddr>>8);//发送高地址	    
	}else I2C_Send_Byte(0XA0+((ReadAddr/256)<<1));   //发送器件地址0XA0,写数据 	   
	I2C_Wait_Ack(); 
    I2C_Send_Byte(ReadAddr%256);   //发送低地址
	I2C_Wait_Ack();	    
	I2C_Start();  	 	   
	I2C_Send_Byte(0XA1);           //进入接收模式			   
	I2C_Wait_Ack();	 
    temp=I2C_Read_Byte(0);		   
    I2C_Stop();//产生一个停止条件	    
	return temp;
}

/**
 * @brief 在AT24CXX指定地址写入一个数据
 *		  WriteAddr  :写入数据的目的地址 
 *		  DataToWrite:要写入的数据
 * @retval none
 * @author halo
 * @date 2022-03-18
 */
void AT24CXX_WriteOneByte(uint16_t WriteAddr,uint8_t DataToWrite)
{				   	  	    																 
    I2C_Start();  
	if(EE_TYPE>AT24C16)
	{
		I2C_Send_Byte(0XA0);	    //发送写命令
		I2C_Wait_Ack();
		I2C_Send_Byte(WriteAddr>>8);//发送高地址	  
	}else I2C_Send_Byte(0XA0+((WriteAddr/256)<<1));   //发送器件地址0XA0,写数据 	 
	I2C_Wait_Ack();	   
    I2C_Send_Byte(WriteAddr%256);   //发送低地址
	I2C_Wait_Ack(); 	 										  		   
	I2C_Send_Byte(DataToWrite);     //发送字节							   
	I2C_Wait_Ack();  		    	   
    I2C_Stop();//产生一个停止条件 
	delay_ms(10);	 
}

/**
 * @brief 检查AT24CXX是否正常
 *		  这里用了24XX的最后一个地址(255)来存储标志字.
 *		  如果用其他24C系列,这个地址要修改
 *		  WriteAddr  :写入数据的目的地址 
 *		  DataToWrite:要写入的数据
 * @retval 1:检测失败
		   0:检测成功
 * @author halo
 * @date 2022-03-18
 */
uint8_t AT24CXX_Check(void)
{
	uint8_t temp;
	temp=AT24CXX_ReadOneByte(255);//避免每次开机都写AT24CXX			   
	if(temp==0X55)return 0;		   
	else//排除第一次初始化的情况
	{
		AT24CXX_WriteOneByte(255,0X55);
	    temp=AT24CXX_ReadOneByte(255);	  
		if(temp==0X55)return 0;
	}
	return 1;											  
}

