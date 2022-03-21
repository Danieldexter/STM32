/*------------------------------------------------ 条件编译 ------------------------------------------------*/
#ifndef __GPIO_H
#define __GPIO_H
/*------------------------------------------------- 头文件 -------------------------------------------------*/
#include "sys.h"
/*------------------------------------------------ 更新说明 ------------------------------------------------*/

/*------------------------------------------------- 结构体 -------------------------------------------------*/

/*------------------------------------------------- 宏定义 -------------------------------------------------*/

/*------------------------------------------------- 输入读取 -------------------------------------------------*/

//#define INPUT_X0 PIout(1)
//#define INPUT_X1 PIout(0)
//#define INPUT_X2 PAout(8)
//#define INPUT_X3 PDout(13)
//#define INPUT_X4 PDout(12)
//#define INPUT_X5 PDout(11)
//#define INPUT_X6 PDout(10)
//#define INPUT_X7 PHout(3)

#define	INPUT_X0_IN	 1		//INPUT_Scan()函数的返回值GPIO.PI4
#define	INPUT_X1_IN	 2		//INPUT_Scan()函数的返回值GPIO.PI5
#define	INPUT_X2_IN	 3		//INPUT_Scan()函数的返回值GPIO.PI6
#define	INPUT_X3_IN	 4		//INPUT_Scan()函数的返回值GPIO.PI7
#define	INPUT_X4_IN	 5		//INPUT_Scan()函数的返回值GPIO.PB7	
#define	INPUT_X5_IN	 6		//INPUT_Scan()函数的返回值GPIO.PB6
#define	INPUT_X6_IN	 7		//INPUT_Scan()函数的返回值GPIO.PB5
#define	INPUT_X7_IN	 8		//INPUT_Scan()函数的返回值GPIO.PB4

#define INPUT_X10 HAL_GPIO_ReadPin(GPIOI,GPIO_PIN_4)	//读取X10-PI4
#define INPUT_X11 HAL_GPIO_ReadPin(GPIOI,GPIO_PIN_5)	//读取X11-PI5
#define INPUT_X12 HAL_GPIO_ReadPin(GPIOI,GPIO_PIN_6)	//读取X12-PI6
#define INPUT_X13 HAL_GPIO_ReadPin(GPIOI,GPIO_PIN_7)	//读取X13-PI7
#define INPUT_X14 HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_7)	//读取X14-PB7
#define INPUT_X15 HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_6)	//读取X15-PB6
#define INPUT_X16 HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_5)	//读取X16-PB5
#define INPUT_X17 HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_4)	//读取X17-PB4

#define	INPUT_X10_IN 9			//INPUT_Scan()函数的返回值GPIO.PI4
#define	INPUT_X11_IN 10			//INPUT_Scan()函数的返回值GPIO.PI5
#define	INPUT_X12_IN 11			//INPUT_Scan()函数的返回值GPIO.PI6
#define	INPUT_X13_IN 12			//INPUT_Scan()函数的返回值GPIO.PI7
#define	INPUT_X14_IN 13			//INPUT_Scan()函数的返回值GPIO.PB7
#define	INPUT_X15_IN 14			//INPUT_Scan()函数的返回值GPIO.PB6
#define	INPUT_X16_IN 15			//INPUT_Scan()函数的返回值GPIO.PB5
#define	INPUT_X17_IN 16			//INPUT_Scan()函数的返回值GPIO.PB4
/*------------------------------------------------- 输出读取 -------------------------------------------------*/

#define OUPUT_Y0   	PHout(8)	//输出Y0 的标志
#define OUPUT_Y1   	PHout(9)	//输出Y1 的标志
#define OUPUT_Y2	PHout(10) 	//输出Y2 的标志
#define OUPUT_Y3	PHout(11)	//输出Y3 的标志
#define OUPUT_Y4	PBout(14)	//输出Y4 的标志
#define OUPUT_Y5	PBout(15)	//输出Y5 的标志
#define OUPUT_Y6	PDout(8)	//输出Y6 的标志
#define OUPUT_Y7	PDout(9)	//输出Y7 的标志
#define OUPUT_Y10  	PAout(15)	//输出Y10的标志
#define OUPUT_Y11	PCout(10)	//输出Y11的标志
#define OUPUT_Y12	PCout(11)	//输出Y12的标志
#define OUPUT_Y13	PCout(12)	//输出Y13的标志
#define OUPUT_Y14	PDout(0)	//输出Y14的标志
#define OUPUT_Y15	PDout(1)	//输出Y15的标志
#define OUPUT_Y16	PDout(2)	//输出Y16的标志
#define OUPUT_Y17	PDout(4 )	//输出Y17的标志

/*------------------------------------------------ 全局变量 ------------------------------------------------*/

/*------------------------------------------------ 函数声明 ------------------------------------------------*/
void mGPIO_Init(void);
void cheak_input(void);
u8 INPUT_Scan(u8 mode1);
void cheak_input2(void);
void input_scan_Poll(void);
/*----------------------------------------------------------------------------------------------------------*/
#endif
