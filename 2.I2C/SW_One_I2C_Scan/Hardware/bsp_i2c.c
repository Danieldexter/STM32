#include "bsp_i2c.h"

//IIC IO 初始化
void I2C_Init(void)
{
	GPIO_InitTypeDef GPIO_Initure;
    
    I2C_SDA_CLK();   //使能GPIOC时钟
    I2C_SCL_CLK();
    //PC11,12初始化设置
    GPIO_Initure.Pin=I2C_SDA_Pin|I2C_SCL_Pin;
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //推挽输出
    GPIO_Initure.Pull=GPIO_PULLUP;          //上拉
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;//高速
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);

	I2C_SDA_H;
	I2C_SCL_H;
}

void SDA_OUT(void)
{
	GPIO_InitTypeDef GPIO_Initure;
    
    __HAL_RCC_GPIOB_CLK_ENABLE();   //使能GPIOC时钟
    
    //PC11初始化设置
    GPIO_Initure.Pin=I2C_SDA_Pin;
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //推挽输出
    GPIO_Initure.Pull=GPIO_PULLUP;          //上拉
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;//高速
    HAL_GPIO_Init(I2C_SDA_Port,&GPIO_Initure);
}

void SDA_IN(void)
{
	GPIO_InitTypeDef GPIO_Initure;
    
    __HAL_RCC_GPIOB_CLK_ENABLE();   //使能GPIOC时钟
    
    //PC11初始化设置
    GPIO_Initure.Pin=I2C_SDA_Pin;
	GPIO_Initure.Pull=GPIO_NOPULL;          //
    GPIO_Initure.Mode=GPIO_MODE_INPUT;  
    HAL_GPIO_Init(I2C_SDA_Port,&GPIO_Initure);
}

/**
 * @brief I2C 开始,SCL为高电平的时候SDA产生一个下降沿信号
 * @retval none
 * @author halo
 * @date 2022-03-18
 */
void I2C_Start(void)
{
	/*    _____
     *SDA      \_____________
     *    __________
     *SCL           \________
     */
	SDA_OUT();	//sda线输出
	I2C_SDA_H;
	I2C_SCL_H;
	delay_us(4);
	I2C_SDA_L;	//START:when CLK is high,DATA change form high to low
	delay_us(4);
	I2C_SCL_L;	//钳住I2C总线，准备发送或接收数据
}	

/**
 * @brief I2C 停止,SCL为高电平的时候SDA产生一个上升沿信号
 * @retval none
 * @author halo
 * @date 2022-03-18
 */
void I2C_Stop(void)
{
	 /*              _______
     *SDA __________/
     *          ____________
     *SCL _____/
     */
	SDA_OUT();
	I2C_SCL_L;
	I2C_SDA_L;//STOP:when CLK is high DATA change form low to high
	delay_us(4);
	I2C_SCL_H;
	I2C_SDA_H;//发送I2C总线结束信号
	delay_us(4);
}

/**
 * @brief I2C 等待响应
 * @retval 1，接收应答失败
		   0，接收应答成功
 * @author halo
 * @date 2022-03-18
 */
uint8_t I2C_Wait_Ack(void)
{
	uint8_t ucErrTime = 0;
	SDA_IN();	//SDA设置为输入
	I2C_SDA_H;delay_us(1);
	I2C_SCL_H;delay_us(1);
	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			I2C_Stop();
			return 1;
		}
	}
	I2C_SCL_L;
	return 0;
}

/**
 * @brief I2C 响应
 * @retval none
 * @author halo
 * @date 2022-03-18
 */
void I2C_Ack(void)
{
	/*           ____
     *SCL ______/    \______
     *    ____         _____
     *SDA     \_______/
     */
	I2C_SCL_L;
	SDA_OUT();	
	I2C_SDA_L;
	delay_us(2);
	I2C_SCL_H;
	delay_us(2);
	I2C_SCL_L;
}

/**
 * @brief I2C 不响应
 * @retval none
 * @author halo
 * @date 2022-03-18
 */
void I2C_NAck(void)
{
	/*           ____
     *SCL ______/    \______
     *    __________________
     *SDA
     */
	I2C_SCL_L;
	SDA_OUT();
	I2C_SDA_H;
	delay_us(2);
	I2C_SCL_H;
	delay_us(2);
	I2C_SCL_L;
}

/**
 * @brief I2C 发送一个字节数据
 * @retval 1，有应答
		   0，无应答	
 * @author halo
 * @date 2022-03-18
 */
void I2C_Send_Byte(uint8_t txd)
{
	uint8_t t;
	SDA_OUT();
	I2C_SCL_L;//拉低时钟开始数据传输
	for(t = 0;t<8;t++)
	{
		if(txd & 0x80)
			I2C_SDA_H;
		else
			I2C_SDA_L;
		txd<<=1;
		delay_us(2);
		I2C_SCL_H;
		delay_us(2);
		I2C_SCL_L;
		delay_us(2);
	}
}

/**
 * @brief I2C 读一个字节数据
 * @retval none
 * @author halo
 * @date 2022-03-18
 */
uint8_t I2C_Read_Byte(unsigned char ack)
{
	unsigned char i,receive = 0;
	SDA_IN();//SDA设置为输入
	for(i = 0;i<8;i++)
	{
		I2C_SCL_L;
		delay_us(2);
		I2C_SCL_H;
		receive=receive<<1;
		if(READ_SDA)receive|=1;
		delay_us(1);
	}
	if(!ack)
		I2C_NAck();//发送nACK
	else
		I2C_Ack(); //发送ACK 
	return receive;
}


