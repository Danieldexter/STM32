#include "led.h"
//初始化PB1为输出.并使能时钟	    
//LED IO初始化
void LED_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOG_CLK_ENABLE();           //开启GPIOG时钟
	__HAL_RCC_GPIOF_CLK_ENABLE();           //开启GPIOG时钟

    GPIO_Initure.Pin=GPIO_PIN_0; 			//PG0
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //推挽输出
    GPIO_Initure.Pull=GPIO_PULLUP;          //上拉
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //高速
    HAL_GPIO_Init(GPIOG,&GPIO_Initure);
	
	GPIO_Initure.Pin=GPIO_PIN_13			//PF13
					|GPIO_PIN_14			//PF14
					|GPIO_PIN_15; 			//PF15
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //推挽输出
    GPIO_Initure.Pull=GPIO_PULLUP;          //上拉
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //高速
	HAL_GPIO_Init(GPIOF,&GPIO_Initure);
	
    HAL_GPIO_WritePin(GPIOG,GPIO_PIN_0,GPIO_PIN_SET);	//PG0置1  
    HAL_GPIO_WritePin(GPIOF,GPIO_PIN_13,GPIO_PIN_SET);	//PF13置1   
    HAL_GPIO_WritePin(GPIOF,GPIO_PIN_14,GPIO_PIN_SET);	//PF14置1   
    HAL_GPIO_WritePin(GPIOF,GPIO_PIN_15,GPIO_PIN_SET);	//PF15置1   
	
}





