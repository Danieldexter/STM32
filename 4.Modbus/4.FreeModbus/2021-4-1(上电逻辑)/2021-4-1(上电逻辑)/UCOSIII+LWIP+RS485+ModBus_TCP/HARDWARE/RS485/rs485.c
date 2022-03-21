#include "rs485.h"
#include "pcf8574.h"
#include "delay.h"


UART_HandleTypeDef USART2_RS485Handler;  //USART2句柄(用于RS485)
TIM_HandleTypeDef TIM2_Handler;

#if EN_USART2_RX   		//如果使能了接收   	  
//接收缓存区 	
u8 RS485_RX_BUF[64];  	//接收缓冲,最大64个字节.
//接收到的数据长度
u8 RS485_RX_CNT=0;  

//void USART2_IRQHandler(void)
//{
//    u8 res;	  
//    if((__HAL_UART_GET_FLAG(&USART2_RS485Handler,UART_FLAG_RXNE)!=RESET))  //接收中断
//	{	 	
//       HAL_UART_Receive(&USART2_RS485Handler,&res,1,1000);
//		  if(RS485_RX_CNT<64)
//		  {
//		   	RS485_RX_BUF[RS485_RX_CNT]=res;		//记录接收到的值
//			  RS485_RX_CNT++;						//接收数据增加1 
//		  } 
//	} 
//}    
#endif

//初始化IO 串口2
//bound:波特率
void RS485_Init(u32 bound)
{
    //GPIO端口设置
	GPIO_InitTypeDef GPIO_Initure;
	
	__HAL_RCC_GPIOA_CLK_ENABLE();			//使能GPIOA时钟
	__HAL_RCC_GPIOD_CLK_ENABLE();			//使能GPIOA时钟
	__HAL_RCC_USART2_CLK_ENABLE();			//使能USART2时钟
	
	GPIO_Initure.Pin=GPIO_PIN_3;			//PA3
	GPIO_Initure.Mode=GPIO_MODE_AF_PP;		//复用推挽输出
	GPIO_Initure.Pull=GPIO_PULLUP;			//上拉
	GPIO_Initure.Speed=GPIO_SPEED_HIGH;		//高速
	GPIO_Initure.Alternate=GPIO_AF7_USART2;	//复用为USART2
	HAL_GPIO_Init(GPIOA,&GPIO_Initure);	   	//初始化PA3
	
    GPIO_Initure.Pin=GPIO_PIN_5;			//PD5
	GPIO_Initure.Mode=GPIO_MODE_AF_PP;		//复用推挽输出
	GPIO_Initure.Pull=GPIO_PULLUP;			//上拉
	GPIO_Initure.Speed=GPIO_SPEED_HIGH;		//高速
	GPIO_Initure.Alternate=GPIO_AF7_USART2;	//复用为USART2
	HAL_GPIO_Init(GPIOD,&GPIO_Initure);	   	//初始化PD5
	
    //USART 初始化设置
	USART2_RS485Handler.Instance=USART2;			        //USART2
	USART2_RS485Handler.Init.BaudRate=bound;		        //波特率
	USART2_RS485Handler.Init.WordLength=UART_WORDLENGTH_8B;	//字长为8位数据格式
	USART2_RS485Handler.Init.StopBits=UART_STOPBITS_1;		//一个停止位
	USART2_RS485Handler.Init.Parity=UART_PARITY_NONE;		//无奇偶校验位
	USART2_RS485Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;	//无硬件流控
	USART2_RS485Handler.Init.Mode=UART_MODE_TX_RX;		    //收发模式
	USART2_RS485Handler.Init.OverSampling = UART_OVERSAMPLING_16;
	HAL_UART_Init(&USART2_RS485Handler);			        //HAL_UART_Init()会使能USART2  
  __HAL_UART_DISABLE_IT(&USART2_RS485Handler,UART_IT_TC);
#if EN_USART2_RX
	__HAL_UART_ENABLE_IT(&USART2_RS485Handler,UART_IT_RXNE);//开启接收中断
	HAL_NVIC_SetPriority(USART2_IRQn,0,1);			        //抢占优先级3，子优先级3
	HAL_NVIC_EnableIRQ(USART2_IRQn);				        //使能USART1中断
#endif	
}

//RS485发送len个字节.
//buf:发送区首地址
//len:发送的字节数(为了和本代码的接收匹配,这里建议不要超过64个字节)
void RS485_Send_Data(u8 *buf,u8 len)
{
	HAL_UART_Transmit(&USART2_RS485Handler,buf,len,1000);//串口2发送数据
	RS485_RX_CNT=0;	 
}
//RS485查询接收到的数据
//buf:接收缓存首地址
//len:读到的数据长度
void RS485_Receive_Data(u8 *buf,u8 *len)
{
	u8 rxlen=RS485_RX_CNT;
	u8 i=0;
	*len=0;				//默认为0
	delay_ms(10);		//等待10ms,连续超过10ms没有接收到一个数据,则认为接收结束
	if(rxlen==RS485_RX_CNT&&rxlen)//接收到了数据,且接收完成了
	{
		for(i=0;i<rxlen;i++)
		{
			buf[i]=RS485_RX_BUF[i];	
		}		
		*len=RS485_RX_CNT;	//记录本次数据长度
		RS485_RX_CNT=0;		//清零
	}
} 

void TIM2_Init(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;

  TIM2_Handler.Instance = TIM2;
  TIM2_Handler.Init.Prescaler = (9000-1);
  TIM2_Handler.Init.CounterMode = TIM_COUNTERMODE_UP;
  TIM2_Handler.Init.Period = 50;
  TIM2_Handler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  HAL_TIM_Base_Init(&TIM2_Handler);

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  HAL_TIM_ConfigClockSource(&TIM2_Handler, &sClockSourceConfig);

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(&TIM2_Handler, &sMasterConfig);  
}

//void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base)
//{

//  if(htim_base->Instance==TIM2)
//  {
//    /* 基本定时器外设时钟使能 */
//    __HAL_RCC_TIM2_CLK_ENABLE();

//    /* 外设中断配置 */
//    HAL_NVIC_SetPriority(TIM2_IRQn, 1, 0);
//    HAL_NVIC_EnableIRQ(TIM2_IRQn);
//  }
//}

/**
  * 函数功能: 基本定时器硬件反初始化配置
  * 输入参数: htim_base：基本定时器句柄类型指针
  * 返 回 值: 无
  * 说    明: 该函数被HAL库内部调用
  */
void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* htim_base)
{

  if(htim_base->Instance==TIM2)
  {
    /* 基本定时器外设时钟禁用 */
    __HAL_RCC_TIM2_CLK_DISABLE();

    /* 关闭外设中断 */
    HAL_NVIC_DisableIRQ(TIM2_IRQn);
  }
} 



