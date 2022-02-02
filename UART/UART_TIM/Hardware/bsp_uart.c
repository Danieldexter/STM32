#include "bsp_uart.h"
#include "bsp_tim.h"
mdrxmsg rxbuf[10] = {0};

uint32_t ulBaudRate = 9600;

void USART1_IRQHandler(void)
{
//    HAL_UART_IRQHandler(&huart1);
	switch(RevSta)
	{
		case RevIDLE:
		{
			HAL_TIM_Base_Start_IT(&htim4);
			RevSta = RevRXNE;
		}
		case RevRXNE:
		{
			__HAL_TIM_SetCounter(&htim4,0);
			if(__HAL_UART_GET_FLAG(&huart1,UART_FLAG_RXNE)!=RESET)	//�����жϴ���
	        {
	          	rxbuf[rxhead].rxmsg[rxbuf[rxhead].len++] = READ_REG(USART1->DR);//��ȡ���ڽ��ͼĴ�������ֵ
		        if(rxbuf[rxhead].len > BUF_LEN)								//���������ݳ��ȴ��ڹ涨���ȣ�����
			    rxbuf[rxhead].len = 0;
	        }   
		}
		break;
		case RevTimeOutError:
		{
			
		}
		break;
	}
	__HAL_UART_CLEAR_FLAG(&huart1,UART_FLAG_RXNE);		//������պ���flag
}

void usart1_byte_send(unsigned char *p, unsigned char len)
{
    while(len--)
    {
        while((USART1->SR&0X40)==0);//ѭ������,ֱ��������� //SR�Ĵ����ĵ���λΪ1�����ʾ�����Ѿ��������
        USART1->DR = (uint8_t) *p++;
    }
}

void usart1_str_send(unsigned char *p)
{
    do
    {
        while((USART1->SR&0X40)==0);//ѭ������,ֱ���������
        USART1->DR = (uint8_t) *p++;
    } while(*p != 0);
}

//len ���� 0 �� �����ַ���
void usart1_send(unsigned char *p,unsigned char len)
{
    if(len)
    {
        usart1_byte_send(p,len);
    }
    else
        do
        {
            while((USART1->SR&0X40)==0);//ѭ������,ֱ���������
            USART1->DR = (uint8_t) *p++;
        } while(*p != 0);
}

