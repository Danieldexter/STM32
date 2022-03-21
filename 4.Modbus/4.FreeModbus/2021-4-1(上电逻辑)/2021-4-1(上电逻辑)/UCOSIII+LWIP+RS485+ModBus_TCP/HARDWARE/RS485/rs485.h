#ifndef __RS485_H
#define __RS485_H
#include "sys.h"	

extern u8 RS485_RX_BUF[64]; 		//���ջ���,���64���ֽ�
extern u8 RS485_RX_CNT;   			//���յ������ݳ���
extern UART_HandleTypeDef USART2_RS485Handler;  //USART2���(����RS485)
extern TIM_HandleTypeDef TIM2_Handler;
//����봮���жϽ��գ�����EN_USART2_RXΪ1����������Ϊ0
#define EN_USART2_RX 	1			//0,������;1,����.
	
void TIM2_Init(void);
void RS485_Init(u32 bound);
void RS485_Send_Data(u8 *buf,u8 len);
void RS485_Receive_Data(u8 *buf,u8 *len);	
void RS485_TX_Set(u8 en);
#endif
