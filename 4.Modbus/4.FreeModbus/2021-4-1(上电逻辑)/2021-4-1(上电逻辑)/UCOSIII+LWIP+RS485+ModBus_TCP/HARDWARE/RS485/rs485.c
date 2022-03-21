#include "rs485.h"
#include "pcf8574.h"
#include "delay.h"


UART_HandleTypeDef USART2_RS485Handler;  //USART2���(����RS485)
TIM_HandleTypeDef TIM2_Handler;

#if EN_USART2_RX   		//���ʹ���˽���   	  
//���ջ����� 	
u8 RS485_RX_BUF[64];  	//���ջ���,���64���ֽ�.
//���յ������ݳ���
u8 RS485_RX_CNT=0;  

//void USART2_IRQHandler(void)
//{
//    u8 res;	  
//    if((__HAL_UART_GET_FLAG(&USART2_RS485Handler,UART_FLAG_RXNE)!=RESET))  //�����ж�
//	{	 	
//       HAL_UART_Receive(&USART2_RS485Handler,&res,1,1000);
//		  if(RS485_RX_CNT<64)
//		  {
//		   	RS485_RX_BUF[RS485_RX_CNT]=res;		//��¼���յ���ֵ
//			  RS485_RX_CNT++;						//������������1 
//		  } 
//	} 
//}    
#endif

//��ʼ��IO ����2
//bound:������
void RS485_Init(u32 bound)
{
    //GPIO�˿�����
	GPIO_InitTypeDef GPIO_Initure;
	
	__HAL_RCC_GPIOA_CLK_ENABLE();			//ʹ��GPIOAʱ��
	__HAL_RCC_GPIOD_CLK_ENABLE();			//ʹ��GPIOAʱ��
	__HAL_RCC_USART2_CLK_ENABLE();			//ʹ��USART2ʱ��
	
	GPIO_Initure.Pin=GPIO_PIN_3;			//PA3
	GPIO_Initure.Mode=GPIO_MODE_AF_PP;		//�����������
	GPIO_Initure.Pull=GPIO_PULLUP;			//����
	GPIO_Initure.Speed=GPIO_SPEED_HIGH;		//����
	GPIO_Initure.Alternate=GPIO_AF7_USART2;	//����ΪUSART2
	HAL_GPIO_Init(GPIOA,&GPIO_Initure);	   	//��ʼ��PA3
	
    GPIO_Initure.Pin=GPIO_PIN_5;			//PD5
	GPIO_Initure.Mode=GPIO_MODE_AF_PP;		//�����������
	GPIO_Initure.Pull=GPIO_PULLUP;			//����
	GPIO_Initure.Speed=GPIO_SPEED_HIGH;		//����
	GPIO_Initure.Alternate=GPIO_AF7_USART2;	//����ΪUSART2
	HAL_GPIO_Init(GPIOD,&GPIO_Initure);	   	//��ʼ��PD5
	
    //USART ��ʼ������
	USART2_RS485Handler.Instance=USART2;			        //USART2
	USART2_RS485Handler.Init.BaudRate=bound;		        //������
	USART2_RS485Handler.Init.WordLength=UART_WORDLENGTH_8B;	//�ֳ�Ϊ8λ���ݸ�ʽ
	USART2_RS485Handler.Init.StopBits=UART_STOPBITS_1;		//һ��ֹͣλ
	USART2_RS485Handler.Init.Parity=UART_PARITY_NONE;		//����żУ��λ
	USART2_RS485Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;	//��Ӳ������
	USART2_RS485Handler.Init.Mode=UART_MODE_TX_RX;		    //�շ�ģʽ
	USART2_RS485Handler.Init.OverSampling = UART_OVERSAMPLING_16;
	HAL_UART_Init(&USART2_RS485Handler);			        //HAL_UART_Init()��ʹ��USART2  
  __HAL_UART_DISABLE_IT(&USART2_RS485Handler,UART_IT_TC);
#if EN_USART2_RX
	__HAL_UART_ENABLE_IT(&USART2_RS485Handler,UART_IT_RXNE);//���������ж�
	HAL_NVIC_SetPriority(USART2_IRQn,0,1);			        //��ռ���ȼ�3�������ȼ�3
	HAL_NVIC_EnableIRQ(USART2_IRQn);				        //ʹ��USART1�ж�
#endif	
}

//RS485����len���ֽ�.
//buf:�������׵�ַ
//len:���͵��ֽ���(Ϊ�˺ͱ�����Ľ���ƥ��,���ｨ�鲻Ҫ����64���ֽ�)
void RS485_Send_Data(u8 *buf,u8 len)
{
	HAL_UART_Transmit(&USART2_RS485Handler,buf,len,1000);//����2��������
	RS485_RX_CNT=0;	 
}
//RS485��ѯ���յ�������
//buf:���ջ����׵�ַ
//len:���������ݳ���
void RS485_Receive_Data(u8 *buf,u8 *len)
{
	u8 rxlen=RS485_RX_CNT;
	u8 i=0;
	*len=0;				//Ĭ��Ϊ0
	delay_ms(10);		//�ȴ�10ms,��������10msû�н��յ�һ������,����Ϊ���ս���
	if(rxlen==RS485_RX_CNT&&rxlen)//���յ�������,�ҽ��������
	{
		for(i=0;i<rxlen;i++)
		{
			buf[i]=RS485_RX_BUF[i];	
		}		
		*len=RS485_RX_CNT;	//��¼�������ݳ���
		RS485_RX_CNT=0;		//����
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
//    /* ������ʱ������ʱ��ʹ�� */
//    __HAL_RCC_TIM2_CLK_ENABLE();

//    /* �����ж����� */
//    HAL_NVIC_SetPriority(TIM2_IRQn, 1, 0);
//    HAL_NVIC_EnableIRQ(TIM2_IRQn);
//  }
//}

/**
  * ��������: ������ʱ��Ӳ������ʼ������
  * �������: htim_base��������ʱ���������ָ��
  * �� �� ֵ: ��
  * ˵    ��: �ú�����HAL���ڲ�����
  */
void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* htim_base)
{

  if(htim_base->Instance==TIM2)
  {
    /* ������ʱ������ʱ�ӽ��� */
    __HAL_RCC_TIM2_CLK_DISABLE();

    /* �ر������ж� */
    HAL_NVIC_DisableIRQ(TIM2_IRQn);
  }
} 



