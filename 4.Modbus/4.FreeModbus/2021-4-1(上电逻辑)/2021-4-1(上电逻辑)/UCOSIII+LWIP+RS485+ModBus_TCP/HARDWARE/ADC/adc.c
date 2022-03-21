#include "adc.h"
#include "delay.h"
#include "user_mb_app.h"
#include "spi.h"
#include "led.h"
#include "gpio.h"

//��������
void Error_Handler(void);
float adc_count(float value,u8 Mode);
void TimingDelay_Decrement(void);
u16 Get_Adc(u32 ch,u8 rank);  
u16 Get_Adc_Average(u32 ch,u8 times,u8 rank);
u16 Get_Adc_Average_c(u32 ch,u8 times,u8 rank);
u16 Get_Adc_Average_T(u32 ch,u8 times,u8 rank);
int num_0(u32 a[],int n);


//�������
ADC_HandleTypeDef ADC1_Handler;//ADC���
ADC_ChannelConfTypeDef sConfig = {0};
ADC_InjectionConfTypeDef sConfigInjected = {0};

//����12λ��ADC��3.3V��ADCֵΪ0xfff,�¶�Ϊ25��ʱ��Ӧ�ĵ�ѹֵΪ1.43V��0x6EE
#define V25  0x6EE
//б�� ÿ���϶�4.3mV ��Ӧÿ���϶�0x05
#define AVG_SLOPE 0x05

#define CC 1
#define CV 1
#define CW 1
#define CR 1


//��ʼ��ADC
//ͨ��ֵ 0~16ȡֵ��ΧΪ��ADC_CHANNEL_0~ADC_CHANNEL_16
void MY_ADC_Init(void)
{ 
    ADC1_Handler.Instance=ADC1;
    ADC1_Handler.Init.ClockPrescaler=ADC_CLOCK_SYNC_PCLK_DIV4;   //4��Ƶ��ADCCLK=PCLK2/4=90/4=22.5MHZ
    ADC1_Handler.Init.Resolution=ADC_RESOLUTION_12B;             //12λģʽ
    ADC1_Handler.Init.DataAlign=ADC_DATAALIGN_RIGHT;             //�Ҷ���
    ADC1_Handler.Init.ScanConvMode=DISABLE;                       //ɨ��ģʽ
    ADC1_Handler.Init.EOCSelection=DISABLE;         			 //�ر�EOC�ж�
    ADC1_Handler.Init.ContinuousConvMode=DISABLE;                //�ر�����ת��
    ADC1_Handler.Init.NbrOfConversion=1;                         //5��ת���ڹ��������� Ҳ����ֻת����������5
    ADC1_Handler.Init.DiscontinuousConvMode=DISABLE;              //����������ģʽ
    ADC1_Handler.Init.NbrOfDiscConversion=0;                     //����������ͨ����Ϊ1
    ADC1_Handler.Init.ExternalTrigConv=ADC_SOFTWARE_START;       //�������
    ADC1_Handler.Init.ExternalTrigConvEdge=ADC_EXTERNALTRIGCONVEDGE_NONE;//ʹ���������
    ADC1_Handler.Init.DMAContinuousRequests=DISABLE;             //�ر�DMA����
    HAL_ADC_Init(&ADC1_Handler);                                 //��ʼ��

//	sConfig.Channel = ADC_CHANNEL_4;							 //����ͨ��ADC_CHANNEL_4->ǰ��ѹ
//	sConfig.Rank = 1;											 //����1
//	sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;				 //����ʱ��
//	if (HAL_ADC_ConfigChannel(&ADC1_Handler, &sConfig) != HAL_OK)
//	{
//		Error_Handler();
//	}
//	/** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
//	*/
//	sConfig.Channel = ADC_CHANNEL_5;							 //����ͨ��ADC_CHANNEL_5->���ѹ
//	sConfig.Rank = 2;											 //����2
//	if (HAL_ADC_ConfigChannel(&ADC1_Handler, &sConfig) != HAL_OK)
//	{
//		Error_Handler();
//	}
//	/** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
//	*/
//	sConfig.Channel = ADC_CHANNEL_0;							 //����ͨ��ADC_CHANNEL_0->ǰ����
//	sConfig.Rank = 3;											 //����3
//	if (HAL_ADC_ConfigChannel(&ADC1_Handler, &sConfig) != HAL_OK)
//	{
//		Error_Handler();
//	}
//		/** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
//	*/
//	sConfig.Channel = ADC_CHANNEL_10;							 //����ͨ��ADC_CHANNEL_10->�����
//	sConfig.Rank = 4;											 //����4
//	if (HAL_ADC_ConfigChannel(&ADC1_Handler, &sConfig) != HAL_OK)
//	{
//		Error_Handler();
//	}
//		/** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
//	*/
//	sConfig.Channel = ADC_CHANNEL_9;							 //����ͨ��ADC_CHANNEL_9->�¶�
//	sConfig.Rank = 5;											 //����5
//	if (HAL_ADC_ConfigChannel(&ADC1_Handler, &sConfig) != HAL_OK)
//	{
//		Error_Handler();
//	}
}

void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

//ADC�ײ��������������ã�ʱ��ʹ��
//�˺����ᱻHAL_ADC_Init()����
//hadc:ADC���
void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_ADC1_CLK_ENABLE();            //ʹ��ADC1ʱ��
    __HAL_RCC_GPIOA_CLK_ENABLE();			//����GPIOAʱ��
	__HAL_RCC_GPIOB_CLK_ENABLE();			//����GPIOBʱ��
	__HAL_RCC_GPIOC_CLK_ENABLE();			//����GPIOBʱ��

    GPIO_Initure.Pin = GPIO_PIN_0 |GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6;//PA4��PA5��PA6
    GPIO_Initure.Mode = GPIO_MODE_ANALOG;   //ģ��
    GPIO_Initure.Pull = GPIO_NOPULL;        //����������
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);
	
	GPIO_Initure.Pin = GPIO_PIN_0 | GPIO_PIN_1;//PB0��PB1
    GPIO_Initure.Mode = GPIO_MODE_ANALOG;   //ģ��
    GPIO_Initure.Pull = GPIO_NOPULL;        //����������
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);
	
	GPIO_Initure.Pin = GPIO_PIN_0 | GPIO_PIN_3;//PC0��PC3
    GPIO_Initure.Mode = GPIO_MODE_ANALOG;   //ģ��
    GPIO_Initure.Pull = GPIO_NOPULL;        //����������
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);
	
}



///***********************************************************************************************************
//**������ADC_Poll();
//**04������ (3x)
//**04 Read Input Registers
//**��ʼ��ַ 300
//**���ܣ�ADC��ѯ��������ADֵд�뵽����Ĵ���buf����ѹֵ���ڲ��¶ȣ�������
///**********************************************************************************************************/
#define IGBT_R 3 		//IGBT��Ч����
#define Totala_Sum 12 	//IGBT��·��
#define CC_IGBT_T 1000		//IGBT����
#define IGBT_T 1000		//IGBT����
#define Vol_full 1000	//��������ѹ���� Vol_full��[0,Vol_full]
#define Current_CHK 200 //�������������� Curerent_CHK��[0,Current_CHK]
extern USHORT   usSRegInBuf[];//����Ĵ���
extern USHORT usSRegHoldBuf[];//���뱣�ּĴ���
extern u8 INPUT_X2;//CC����
extern u8 INPUT_X3;//CV����
extern u8 INPUT_X4;//CW����
extern u8 INPUT_X5;//CR����
extern u8 INPUT_X2_FLAG;//CC���ر�־λ
extern u8 INPUT_X3_FLAG;//CV���ر�־λ
extern u8 INPUT_X4_FLAG;//CW���ر�־λ
extern u8 INPUT_X5_FLAG;//CR���ر�־λ
u16 CC_UP=0;
u16 CV_UP=0;
u16 CR_UP=0;
u16 CP_UP=0;
u8 CC_UP_FLAG=0;
u8 CV_UP_FLAG=0;
u8 CR_UP_FLAG=0;
u8 CP_UP_FLAG=0;
uint16_t ADD_HOT_SEND1=0;
uint16_t ADD_HOT_SEND=0;  //��������
uint16_t ADD_HOT_SEND_TEMP=0;  //��������
uint16_t ADD_HOT_Receive=0; //��������
uint16_t Voltage_ON=0;//��ǰ��ѹ
uint16_t Current_ON=0; //��ǰ����
uint16_t Temperature_ON=0; //��ǰ�¶�
uint16_t P_ON=0;//��ǰ����
uint16_t Current_SET=0; //�趨����
uint16_t V_I_SET=0;
uint16_t P_U_SET=0;
uint16_t R_I_SET=0;
__IO u32 TimingDelay;
__IO u32 TimingDelay1;
__IO u32 TimingDelay2;
__IO u32 TimingDelay3;
uint16_t C_SET1=0;
uint16_t V_SET1=0;
uint16_t R_SET1=0;
uint16_t P_SET1=0;
uint16_t TEXT_FINISH1;
uint16_t TEXT_FINISH2;
uint16_t TEXT_FINISH3;
uint16_t TEXT_FINISH4;
uint16_t mode;
float REVoltage;
float Current;
float Temp;
u32 iiiiii;

void ADC_Poll(void)
{	
	u8 i;
	uint32_t adcBuf[5]={0,0,0,0,0};

	/***********************************ADC����**********************************************/
//	for(i=0;i<5;i++)
//	{
//		HAL_ADC_Start(&ADC1_Handler);					 //����ADC				
//		HAL_ADC_PollForConversion(&ADC1_Handler,0xffff); //�ȴ�ADCת�����
//		adcBuf[i]=HAL_ADC_GetValue(&ADC1_Handler);		 //�������һ��ADC1�������ת�����
//	}
	adcBuf[1]=Get_Adc_Average(ADC_CHANNEL_5,70,1);//��ѹ
	adcBuf[2]=Get_Adc_Average_c(ADC_CHANNEL_10,70,2);//����
	adcBuf[4]=Get_Adc_Average_T(ADC_CHANNEL_9,50,3);//�¶�
	adcBuf[0]=Get_Adc_Average_T(ADC_CHANNEL_4,50,4);

	REVoltage=adc_count(adcBuf[1],1)-3.9756;
	Current=adc_count(adcBuf[2],2)+1.4;
	Temp=adc_count(adcBuf[4],3);	
	if(REVoltage>0 && Current>0)
	{
		P_ON=REVoltage*Current;
	}
	if(Temp>400)
	{
		Temp=0;
	}
											//��ַ��Ӧ����			��Ļ��ַ��		
	usSRegInBuf[0] = Current*10;			//������ʾ*10 			301
	usSRegInBuf[1] = adcBuf[0]*33/4096;		//��·��ǰ��ѹ��ʾ*10 	302
	usSRegInBuf[2] = REVoltage*10; 			//��·�����ѹ��ʾ*10 	303
	usSRegInBuf[3] = Temp*10;				//�¶���ʾ*10 			304
	usSRegInBuf[4] = Current;				//������ʾ 				305
	usSRegInBuf[5] = adcBuf[0]*3.3/4096;	//��·��ǰ��ѹ��ʾ 		306
	usSRegInBuf[6] = REVoltage;				//��·�����ѹ��ʾ 		307
	usSRegInBuf[7] = Temp*10;				//�¶���ʾ 				308
	usSRegInBuf[8] = P_ON*10; 				//������ʾ*10			309
	usSRegInBuf[9] = P_ON; 					//������ʾ 				310
	HAL_ADC_Stop(&ADC1_Handler);
	/**********************************�˶�ͨѶֵ**********************************************/
	if(REVoltage<650) //����ֵ650.0
	{
		Voltage_ON=REVoltage;//��ѹ�Ƚ����			 
	}
	else if(REVoltage>650)
	{
		OUPUT_Y3=0;
	}
	if(Current<850)//����ֵ850.0
	{
		Current_ON=Current;//�����Ƚ����				 
	}
	else if(Current>850)
	{
		OUPUT_Y3=0;
	}
	if(Temp<400)//����ֵ400.0
	{
		Temperature_ON=Temp;//�¶ȱȽ����				 
	}
	else if(Temp>400)
	{
		OUPUT_Y3=0;
	}
	/*************************************CCģʽ���*****************************************/
	if(INPUT_X2==CC)//CC���ؿ���
	{
		C_SET1=usSRegHoldBuf[1];
		mode=0x01;
		//�˶�ͨѶֵ				 
		if(C_SET1<850)//����ֵ850.0
		{
			Current_SET=C_SET1;//��������Ƚ����				 
		}			 
		//��������Ŀ��ֵ				   
		ADD_HOT_SEND_TEMP=(((IGBT_R*Current_SET*IGBT_T)/Voltage_ON)/Totala_Sum);//ռ�ձ� ��������Ч����*�趨������/�ɼ���ѹ��/·����*����
		if(CC_UP_FLAG==0)
		{
			CC_UP+=10;
			if(CC_UP==ADD_HOT_SEND_TEMP)
			{
				CC_UP_FLAG=1;
			}
			ADD_HOT_SEND=CC_UP;
		}
		else ADD_HOT_SEND=ADD_HOT_SEND_TEMP;
//		ADD_HOT_SEND=ADD_HOT_SEND_TEMP;//�趨���ۼ���ֵ	 
	}
	/*************************************CVģʽ���*****************************************/
	if(INPUT_X3==CV)//CV���ؿ���
	{
		V_SET1=usSRegHoldBuf[2];//�趨��ѹ
		mode=0x02;
		ADD_HOT_SEND_TEMP=(V_SET1*IGBT_T)/Voltage_ON;//Ŀ��Kֵ  ռ�ձ�=�趨��ѹ/�ɼ���ѹ=����Ч����*(����++))/�ɼ���ѹ
		if(CV_UP_FLAG==0)
		{
			CV_UP+=10;
			if(CV_UP==ADD_HOT_SEND_TEMP)
			{
				CV_UP_FLAG=1;
			}
			ADD_HOT_SEND=CV_UP;
		}
		else ADD_HOT_SEND=ADD_HOT_SEND_TEMP;
		//		ADD_HOT_SEND=ADD_HOT_SEND_TEMP;
	}
	/*************************************CWģʽ���*****************************************/
	if(INPUT_X4==CW)//CW���ؿ���
	{
		P_SET1=usSRegHoldBuf[4];//�趨����
		mode=0x03;
		P_U_SET=sqrt((P_SET1*IGBT_R/Totala_Sum))*IGBT_T;
		ADD_HOT_SEND_TEMP=(P_U_SET/Voltage_ON);//Ŀ��Kֵ  ռ�ձ�=�趨��ѹ/�ɼ���ѹ=���趨����/(����++))/�ɼ���ѹ*1000
		if(CP_UP_FLAG==0)
		{
			CP_UP+=10;
			if(CP_UP==ADD_HOT_SEND_TEMP)
			{
				CP_UP_FLAG=1;
			}
			ADD_HOT_SEND=CP_UP;
		}
		else ADD_HOT_SEND=ADD_HOT_SEND_TEMP;
//		ADD_HOT_SEND=ADD_HOT_SEND_TEMP;//�趨���ۼ���ֵ	 
	}
	/*************************************CRģʽ���*****************************************/
	if(INPUT_X5==CR)//CR���ؿ���
	{
		R_SET1=usSRegHoldBuf[3];//�趨��ֵ
		mode=0x04;
		ADD_HOT_SEND_TEMP=(IGBT_R/R_SET1)/Totala_Sum*IGBT_T;//ռ�ձ�=�趨��ѹ/�ɼ���ѹ=����Ч����/�趨��ֵ)/��·��*T	
		if(CR_UP_FLAG==0)
		{
			CR_UP+=10;
			if(CR_UP==ADD_HOT_SEND_TEMP)
			{
				CR_UP_FLAG=1;
			}
			ADD_HOT_SEND=CR_UP;
		}
		else ADD_HOT_SEND=ADD_HOT_SEND_TEMP;
		ADD_HOT_SEND=ADD_HOT_SEND_TEMP;//�趨���ۼ���ֵ
	}
	/*************************************SPI����*****************************************/
	if(ADD_HOT_SEND>1001)//ռ�ձȲ��ܴ���100%
	{
		ADD_HOT_SEND=1001;
	}
			
	if(ADD_HOT_SEND<1)
	{
		ADD_HOT_SEND=0;
	}
	ADD_HOT_SEND1=mode*10000+ADD_HOT_SEND;
	if(TimingDelay1==0)
	{				
		/* �������� */
		TEXT_FINISH2=SPI4_ReadWriteByte(ADD_HOT_SEND1);
		LED0=!LED0;
		TimingDelay1=2; //��λ��ʱ��
	} 
}

//Slave mode:HoldingRegister variables
//************************************************************************//
//**03 06 16������ 
//**03 Read  Holding Registers(4x)
//**06 Write Single Registers
//**16 Write Multiple Registers
//**��ʼ��ַ 400
//************************************************************************//

float adc_count(float value,u8 Mode)
{
	switch(Mode)
	{
		case 1:
			value=value*3.3/4096;			//4-20ma�ɼ���ѹ����
			value=value/0.164;	//
			value=((value-4)*1000)/16;		//���ѹ����
			return value;
		case 2:
			value=value*3.3/4096;			//4-20ma�ɼ���������
			value=value/0.164;		//
			value=((value-4)*200)/16;		//���������
			return value;
		case 3:
			value=value*3.3/4096;
			value=value-0.1;
			if(value>0.001)
			{
				value=value*(300/2.3);//�¶Ȼ���
			}
			return value;
	}
}

extern void TimingDelay_Decrement(void)
{
	if (TimingDelay != 0x00)
	{ 
		TimingDelay--;
	}
	
	if (TimingDelay1 != 0x00)
	{ 
		TimingDelay1--;
	}
	
	if (TimingDelay2 != 0x00)
	{ 
		TimingDelay2--;
	}
	
	if (TimingDelay3 != 0x00)
	{ 
		TimingDelay3--;
	}
}

u16 Get_Adc(u32 ch,u8 rank)   
{
//    ADC_ChannelConfTypeDef sConfig;
    
    sConfig.Channel=ch;                                   //ͨ��
    sConfig.Rank=1;                                       //��1�����У�����1
    sConfig.SamplingTime=ADC_SAMPLETIME_480CYCLES;        //����ʱ��
    sConfig.Offset=0;                 
    HAL_ADC_ConfigChannel(&ADC1_Handler,&sConfig);        //ͨ������
	
    HAL_ADC_Start(&ADC1_Handler);                               //����ADC
	
    HAL_ADC_PollForConversion(&ADC1_Handler,10);                //��ѯת��
 
	return (u16)HAL_ADC_GetValue(&ADC1_Handler);	        //�������һ��ADC1�������ת�����
}

u16 Get_Adc_Average(u32 ch,u8 times,u8 rank)
{
	u32 temp_val=0;
	u8 t;
	u32 num[times];
	u32 test;
	for(t=0;t<times;t++)
	{
		num[t]=Get_Adc(ch,rank);
		temp_val+=Get_Adc(ch,rank);
	}
	temp_val=temp_val/times;
	test=temp_val/33;
	return temp_val;
} 

u16 Get_Adc_Average_c(u32 ch,u8 times,u8 rank)
{
	u32 temp_val=0;
	u8 t;
	u32 num[times];
	u32 test;
	u8 temp_0;	//0�ĸ���
	for(t=0;t<times;t++)
	{
		num[t]=Get_Adc(ch,rank);
		temp_val+=Get_Adc(ch,rank);
	}
	temp_0=num_0(num,70);				//�������ݲ���
	temp_val=temp_val/(times-temp_0);	//ע�͵������У�ȥ��0�ĸ����ٳ�
	//temp_val=temp_val/times;			//�������
	test=temp_val/33;
	return temp_val-test;
} 

u16 Get_Adc_Average_T(u32 ch,u8 times,u8 rank)
{
	u32 temp_val=0;
	u8 t;
	u32 num[times];
	for(t=0;t<times;t++)
	{
		num[t]=Get_Adc(ch,rank);
		temp_val+=Get_Adc(ch,rank);
	}
	return temp_val/times;
} 

int num_0(u32 a[],int n)
{
	int sum=0;
	int i_0=0;
	for(i_0=0;i_0<n;i_0++)
	if(a[i_0]==0)sum++;
	return sum;
}
