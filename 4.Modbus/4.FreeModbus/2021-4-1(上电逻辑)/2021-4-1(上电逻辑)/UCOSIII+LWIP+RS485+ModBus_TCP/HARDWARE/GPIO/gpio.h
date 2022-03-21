/*------------------------------------------------ �������� ------------------------------------------------*/
#ifndef __GPIO_H
#define __GPIO_H
/*------------------------------------------------- ͷ�ļ� -------------------------------------------------*/
#include "sys.h"
/*------------------------------------------------ ����˵�� ------------------------------------------------*/

/*------------------------------------------------- �ṹ�� -------------------------------------------------*/

/*------------------------------------------------- �궨�� -------------------------------------------------*/

/*------------------------------------------------- �����ȡ -------------------------------------------------*/

//#define INPUT_X0 PIout(1)
//#define INPUT_X1 PIout(0)
//#define INPUT_X2 PAout(8)
//#define INPUT_X3 PDout(13)
//#define INPUT_X4 PDout(12)
//#define INPUT_X5 PDout(11)
//#define INPUT_X6 PDout(10)
//#define INPUT_X7 PHout(3)

#define	INPUT_X0_IN	 1		//INPUT_Scan()�����ķ���ֵGPIO.PI4
#define	INPUT_X1_IN	 2		//INPUT_Scan()�����ķ���ֵGPIO.PI5
#define	INPUT_X2_IN	 3		//INPUT_Scan()�����ķ���ֵGPIO.PI6
#define	INPUT_X3_IN	 4		//INPUT_Scan()�����ķ���ֵGPIO.PI7
#define	INPUT_X4_IN	 5		//INPUT_Scan()�����ķ���ֵGPIO.PB7	
#define	INPUT_X5_IN	 6		//INPUT_Scan()�����ķ���ֵGPIO.PB6
#define	INPUT_X6_IN	 7		//INPUT_Scan()�����ķ���ֵGPIO.PB5
#define	INPUT_X7_IN	 8		//INPUT_Scan()�����ķ���ֵGPIO.PB4

#define INPUT_X10 HAL_GPIO_ReadPin(GPIOI,GPIO_PIN_4)	//��ȡX10-PI4
#define INPUT_X11 HAL_GPIO_ReadPin(GPIOI,GPIO_PIN_5)	//��ȡX11-PI5
#define INPUT_X12 HAL_GPIO_ReadPin(GPIOI,GPIO_PIN_6)	//��ȡX12-PI6
#define INPUT_X13 HAL_GPIO_ReadPin(GPIOI,GPIO_PIN_7)	//��ȡX13-PI7
#define INPUT_X14 HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_7)	//��ȡX14-PB7
#define INPUT_X15 HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_6)	//��ȡX15-PB6
#define INPUT_X16 HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_5)	//��ȡX16-PB5
#define INPUT_X17 HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_4)	//��ȡX17-PB4

#define	INPUT_X10_IN 9			//INPUT_Scan()�����ķ���ֵGPIO.PI4
#define	INPUT_X11_IN 10			//INPUT_Scan()�����ķ���ֵGPIO.PI5
#define	INPUT_X12_IN 11			//INPUT_Scan()�����ķ���ֵGPIO.PI6
#define	INPUT_X13_IN 12			//INPUT_Scan()�����ķ���ֵGPIO.PI7
#define	INPUT_X14_IN 13			//INPUT_Scan()�����ķ���ֵGPIO.PB7
#define	INPUT_X15_IN 14			//INPUT_Scan()�����ķ���ֵGPIO.PB6
#define	INPUT_X16_IN 15			//INPUT_Scan()�����ķ���ֵGPIO.PB5
#define	INPUT_X17_IN 16			//INPUT_Scan()�����ķ���ֵGPIO.PB4
/*------------------------------------------------- �����ȡ -------------------------------------------------*/

#define OUPUT_Y0   	PHout(8)	//���Y0 �ı�־
#define OUPUT_Y1   	PHout(9)	//���Y1 �ı�־
#define OUPUT_Y2	PHout(10) 	//���Y2 �ı�־
#define OUPUT_Y3	PHout(11)	//���Y3 �ı�־
#define OUPUT_Y4	PBout(14)	//���Y4 �ı�־
#define OUPUT_Y5	PBout(15)	//���Y5 �ı�־
#define OUPUT_Y6	PDout(8)	//���Y6 �ı�־
#define OUPUT_Y7	PDout(9)	//���Y7 �ı�־
#define OUPUT_Y10  	PAout(15)	//���Y10�ı�־
#define OUPUT_Y11	PCout(10)	//���Y11�ı�־
#define OUPUT_Y12	PCout(11)	//���Y12�ı�־
#define OUPUT_Y13	PCout(12)	//���Y13�ı�־
#define OUPUT_Y14	PDout(0)	//���Y14�ı�־
#define OUPUT_Y15	PDout(1)	//���Y15�ı�־
#define OUPUT_Y16	PDout(2)	//���Y16�ı�־
#define OUPUT_Y17	PDout(4 )	//���Y17�ı�־

/*------------------------------------------------ ȫ�ֱ��� ------------------------------------------------*/

/*------------------------------------------------ �������� ------------------------------------------------*/
void mGPIO_Init(void);
void cheak_input(void);
u8 INPUT_Scan(u8 mode1);
void cheak_input2(void);
void input_scan_Poll(void);
/*----------------------------------------------------------------------------------------------------------*/
#endif
