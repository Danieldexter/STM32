#include "led.h"
//��ʼ��PB1Ϊ���.��ʹ��ʱ��	    
//LED IO��ʼ��
void LED_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOG_CLK_ENABLE();           //����GPIOGʱ��
	__HAL_RCC_GPIOF_CLK_ENABLE();           //����GPIOGʱ��

    GPIO_Initure.Pin=GPIO_PIN_0; 			//PG0
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //�������
    GPIO_Initure.Pull=GPIO_PULLUP;          //����
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //����
    HAL_GPIO_Init(GPIOG,&GPIO_Initure);
	
	GPIO_Initure.Pin=GPIO_PIN_13			//PF13
					|GPIO_PIN_14			//PF14
					|GPIO_PIN_15; 			//PF15
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //�������
    GPIO_Initure.Pull=GPIO_PULLUP;          //����
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //����
	HAL_GPIO_Init(GPIOF,&GPIO_Initure);
	
    HAL_GPIO_WritePin(GPIOG,GPIO_PIN_0,GPIO_PIN_SET);	//PG0��1  
    HAL_GPIO_WritePin(GPIOF,GPIO_PIN_13,GPIO_PIN_SET);	//PF13��1   
    HAL_GPIO_WritePin(GPIOF,GPIO_PIN_14,GPIO_PIN_SET);	//PF14��1   
    HAL_GPIO_WritePin(GPIOF,GPIO_PIN_15,GPIO_PIN_SET);	//PF15��1   
	
}





