#include "bsp_uart.h"
#include "ringbuffer.h"
mdrxmsg rxbuf[10];

struct rt_ringbuffer ring_buf;      //����1���ջ��ζ��� 
static uint8_t 	USART1_Len = 0;     //��ŵ�ǰ���ڽ������ݴ�ŵ�λ��
uint8_t  s_USART1_RxBuf[256];//���ڽ��ջ�����

void USART1_IRQHandler(void)
{
	uint8_t receive_char;
	
	if((__HAL_UART_GET_FLAG(&huart1, UART_FLAG_RXNE) != RESET)) //���շǿ��ж�
    {
		HAL_UART_Receive(&huart1, &receive_char, 1, 1000);//���յ������ݷ���receive_char		
		rt_ringbuffer_put(&ring_buf,&receive_char,1);//��receive_charѹ�뵽ring_buf��
		USART1_Len++;		
		__HAL_UART_CLEAR_FLAG(&huart1,UART_FLAG_RXNE);//������շǿ��ж�
	}
	
	if((__HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE) != RESET)) //���տ����ж�
    {
	
		rt_ringbuffer_get(&ring_buf,s_USART1_RxBuf, USART1_Len);//��rring_buf�����ݶ�������s_USART1_RxBuf��
		s_USART1_RxBuf[USART1_Len] = '\0';
		usart1_str_send(s_USART1_RxBuf);
		USART1_Len = 0 ;//����֡���ȸ�λ
		__HAL_UART_CLEAR_IDLEFLAG(&huart1);
	}
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

