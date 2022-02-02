#include "bsp_uart.h"
#include "ringbuffer.h"
mdrxmsg rxbuf[10];

struct rt_ringbuffer ring_buf;      //串口1接收环形队列 
static uint8_t 	USART1_Len = 0;     //存放当前串口接收数据存放的位置
uint8_t  s_USART1_RxBuf[256];//串口接收缓冲区

void USART1_IRQHandler(void)
{
	uint8_t receive_char;
	
	if((__HAL_UART_GET_FLAG(&huart1, UART_FLAG_RXNE) != RESET)) //接收非空中断
    {
		HAL_UART_Receive(&huart1, &receive_char, 1, 1000);//将收到的数据放入receive_char		
		rt_ringbuffer_put(&ring_buf,&receive_char,1);//将receive_char压入到ring_buf中
		USART1_Len++;		
		__HAL_UART_CLEAR_FLAG(&huart1,UART_FLAG_RXNE);//清除接收非空中断
	}
	
	if((__HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE) != RESET)) //接收空闲中断
    {
	
		rt_ringbuffer_get(&ring_buf,s_USART1_RxBuf, USART1_Len);//将rring_buf的数据读出到到s_USART1_RxBuf中
		s_USART1_RxBuf[USART1_Len] = '\0';
		usart1_str_send(s_USART1_RxBuf);
		USART1_Len = 0 ;//数据帧长度复位
		__HAL_UART_CLEAR_IDLEFLAG(&huart1);
	}
}


















void usart1_byte_send(unsigned char *p, unsigned char len)
{
    while(len--)
    {
        while((USART1->SR&0X40)==0);//循环发送,直到发送完毕 //SR寄存器的第七位为1，则表示数据已经发送完成
        USART1->DR = (uint8_t) *p++;
    }
}

void usart1_str_send(unsigned char *p)
{
    do
    {
        while((USART1->SR&0X40)==0);//循环发送,直到发送完毕
        USART1->DR = (uint8_t) *p++;
    } while(*p != 0);
}

//len 等于 0 ， 则发送字符串
void usart1_send(unsigned char *p,unsigned char len)
{
    if(len)
    {
        usart1_byte_send(p,len);
    }
    else
        do
        {
            while((USART1->SR&0X40)==0);//循环发送,直到发送完毕
            USART1->DR = (uint8_t) *p++;
        } while(*p != 0);
}

