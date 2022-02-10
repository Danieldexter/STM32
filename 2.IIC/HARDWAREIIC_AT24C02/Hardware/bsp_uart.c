#include "bsp_uart.h"

mdrxmsg rxbuf[10];


void USART1_IRQHandler(void)
{
    /* USER CODE BEGIN USART1_IRQn 0 */

    /* USER CODE END USART1_IRQn 0 */
    HAL_UART_IRQHandler(&huart1);
    /* USER CODE BEGIN USART1_IRQn 1 */
	
	if(__HAL_UART_GET_FLAG(&huart1,UART_FLAG_RXNE)!=RESET)
	{
		rxbuf[rxhead].rxmsg[rxbuf[rxhead].len++] = READ_REG(USART1->DR);
		if(rxbuf[rxhead].len > BUF_LEN)
			rxbuf[rxhead].len = 0;
		__HAL_UART_CLEAR_FLAG(&huart1,UART_FLAG_RXNE);
	}
	if(__HAL_UART_GET_FLAG(&huart1,UART_FLAG_IDLE)!=RESET)
	{
		if(++rxhead >= 10 )
			rxhead = 0;
		__HAL_UART_CLEAR_IDLEFLAG(&huart1);
	}
    /* USER CODE END USART1_IRQn 1 */
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

