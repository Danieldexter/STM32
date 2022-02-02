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
			if(__HAL_UART_GET_FLAG(&huart1,UART_FLAG_RXNE)!=RESET)	//接收中断处理
	        {
	          	rxbuf[rxhead].rxmsg[rxbuf[rxhead].len++] = READ_REG(USART1->DR);//获取串口接送寄存器的数值
		        if(rxbuf[rxhead].len > BUF_LEN)								//当接收数据长度大于规定长度，清零
			    rxbuf[rxhead].len = 0;
	        }   
		}
		break;
		case RevTimeOutError:
		{
			
		}
		break;
	}
	__HAL_UART_CLEAR_FLAG(&huart1,UART_FLAG_RXNE);		//清除接收函数flag
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

