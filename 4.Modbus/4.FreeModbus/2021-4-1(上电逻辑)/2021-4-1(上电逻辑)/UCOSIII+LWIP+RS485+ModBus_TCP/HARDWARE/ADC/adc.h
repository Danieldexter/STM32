#ifndef __ADC_H
#define __ADC_H
#include "sys.h"

#define ADC_CH5  		5 		 	//通道5	  
extern u16 CC_UP;
extern u16 CV_UP;
extern u16 CR_UP;
extern u16 CP_UP;
extern u8 CC_UP_FLAG;
extern u8 CV_UP_FLAG;
extern u8 CR_UP_FLAG;
extern u8 CP_UP_FLAG;
extern uint16_t ADD_HOT_SEND_TEMP;
extern uint16_t ADD_HOT_SEND;
extern uint16_t mode;

void MY_ADC_Init(void); 				//ADC通道初始化
u16 Get_Adc(u32 ch,u8 rank);  
u16 Get_Adc_Average(u32 ch,u8 times,u8 rank);
short Get_Temprate(void);			//得到温度值
void ADC_Poll(void);
void TimingDelay_Decrement(void);

#endif 
