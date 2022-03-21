#include "adc.h"
#include "delay.h"
#include "user_mb_app.h"
#include "spi.h"
#include "led.h"
#include "gpio.h"

//函数声明
void Error_Handler(void);
float adc_count(float value,u8 Mode);
void TimingDelay_Decrement(void);
u16 Get_Adc(u32 ch,u8 rank);  
u16 Get_Adc_Average(u32 ch,u8 times,u8 rank);
u16 Get_Adc_Average_c(u32 ch,u8 times,u8 rank);
u16 Get_Adc_Average_T(u32 ch,u8 times,u8 rank);
int num_0(u32 a[],int n);


//句柄声明
ADC_HandleTypeDef ADC1_Handler;//ADC句柄
ADC_ChannelConfTypeDef sConfig = {0};
ADC_InjectionConfTypeDef sConfigInjected = {0};

//对于12位的ADC，3.3V的ADC值为0xfff,温度为25度时对应的电压值为1.43V即0x6EE
#define V25  0x6EE
//斜率 每摄氏度4.3mV 对应每摄氏度0x05
#define AVG_SLOPE 0x05

#define CC 1
#define CV 1
#define CW 1
#define CR 1


//初始化ADC
//通道值 0~16取值范围为：ADC_CHANNEL_0~ADC_CHANNEL_16
void MY_ADC_Init(void)
{ 
    ADC1_Handler.Instance=ADC1;
    ADC1_Handler.Init.ClockPrescaler=ADC_CLOCK_SYNC_PCLK_DIV4;   //4分频，ADCCLK=PCLK2/4=90/4=22.5MHZ
    ADC1_Handler.Init.Resolution=ADC_RESOLUTION_12B;             //12位模式
    ADC1_Handler.Init.DataAlign=ADC_DATAALIGN_RIGHT;             //右对齐
    ADC1_Handler.Init.ScanConvMode=DISABLE;                       //扫描模式
    ADC1_Handler.Init.EOCSelection=DISABLE;         			 //关闭EOC中断
    ADC1_Handler.Init.ContinuousConvMode=DISABLE;                //关闭连续转换
    ADC1_Handler.Init.NbrOfConversion=1;                         //5个转换在规则序列中 也就是只转换规则序列5
    ADC1_Handler.Init.DiscontinuousConvMode=DISABLE;              //不连续采样模式
    ADC1_Handler.Init.NbrOfDiscConversion=0;                     //不连续采样通道数为1
    ADC1_Handler.Init.ExternalTrigConv=ADC_SOFTWARE_START;       //软件触发
    ADC1_Handler.Init.ExternalTrigConvEdge=ADC_EXTERNALTRIGCONVEDGE_NONE;//使用软件触发
    ADC1_Handler.Init.DMAContinuousRequests=DISABLE;             //关闭DMA请求
    HAL_ADC_Init(&ADC1_Handler);                                 //初始化

//	sConfig.Channel = ADC_CHANNEL_4;							 //开启通道ADC_CHANNEL_4->前电压
//	sConfig.Rank = 1;											 //序列1
//	sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;				 //采样时间
//	if (HAL_ADC_ConfigChannel(&ADC1_Handler, &sConfig) != HAL_OK)
//	{
//		Error_Handler();
//	}
//	/** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
//	*/
//	sConfig.Channel = ADC_CHANNEL_5;							 //开启通道ADC_CHANNEL_5->后电压
//	sConfig.Rank = 2;											 //序列2
//	if (HAL_ADC_ConfigChannel(&ADC1_Handler, &sConfig) != HAL_OK)
//	{
//		Error_Handler();
//	}
//	/** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
//	*/
//	sConfig.Channel = ADC_CHANNEL_0;							 //开启通道ADC_CHANNEL_0->前电流
//	sConfig.Rank = 3;											 //序列3
//	if (HAL_ADC_ConfigChannel(&ADC1_Handler, &sConfig) != HAL_OK)
//	{
//		Error_Handler();
//	}
//		/** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
//	*/
//	sConfig.Channel = ADC_CHANNEL_10;							 //开启通道ADC_CHANNEL_10->后电流
//	sConfig.Rank = 4;											 //序列4
//	if (HAL_ADC_ConfigChannel(&ADC1_Handler, &sConfig) != HAL_OK)
//	{
//		Error_Handler();
//	}
//		/** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
//	*/
//	sConfig.Channel = ADC_CHANNEL_9;							 //开启通道ADC_CHANNEL_9->温度
//	sConfig.Rank = 5;											 //序列5
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

//ADC底层驱动，引脚配置，时钟使能
//此函数会被HAL_ADC_Init()调用
//hadc:ADC句柄
void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_ADC1_CLK_ENABLE();            //使能ADC1时钟
    __HAL_RCC_GPIOA_CLK_ENABLE();			//开启GPIOA时钟
	__HAL_RCC_GPIOB_CLK_ENABLE();			//开启GPIOB时钟
	__HAL_RCC_GPIOC_CLK_ENABLE();			//开启GPIOB时钟

    GPIO_Initure.Pin = GPIO_PIN_0 |GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6;//PA4、PA5、PA6
    GPIO_Initure.Mode = GPIO_MODE_ANALOG;   //模拟
    GPIO_Initure.Pull = GPIO_NOPULL;        //不带上下拉
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);
	
	GPIO_Initure.Pin = GPIO_PIN_0 | GPIO_PIN_1;//PB0、PB1
    GPIO_Initure.Mode = GPIO_MODE_ANALOG;   //模拟
    GPIO_Initure.Pull = GPIO_NOPULL;        //不带上下拉
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);
	
	GPIO_Initure.Pin = GPIO_PIN_0 | GPIO_PIN_3;//PC0、PC3
    GPIO_Initure.Mode = GPIO_MODE_ANALOG;   //模拟
    GPIO_Initure.Pull = GPIO_NOPULL;        //不带上下拉
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);
	
}



///***********************************************************************************************************
//**函数：ADC_Poll();
//**04功能码 (3x)
//**04 Read Input Registers
//**起始地址 300
//**功能：ADC轮询函数，将AD值写入到输入寄存器buf，电压值，内部温度，光亮度
///**********************************************************************************************************/
#define IGBT_R 3 		//IGBT等效电阻
#define Totala_Sum 12 	//IGBT总路数
#define CC_IGBT_T 1000		//IGBT周期
#define IGBT_T 1000		//IGBT周期
#define Vol_full 1000	//变送器电压量程 Vol_full∈[0,Vol_full]
#define Current_CHK 200 //变送器电流量程 Curerent_CHK∈[0,Current_CHK]
extern USHORT   usSRegInBuf[];//输入寄存器
extern USHORT usSRegHoldBuf[];//输入保持寄存器
extern u8 INPUT_X2;//CC开关
extern u8 INPUT_X3;//CV开关
extern u8 INPUT_X4;//CW开关
extern u8 INPUT_X5;//CR开关
extern u8 INPUT_X2_FLAG;//CC开关标志位
extern u8 INPUT_X3_FLAG;//CV开关标志位
extern u8 INPUT_X4_FLAG;//CW开关标志位
extern u8 INPUT_X5_FLAG;//CR开关标志位
u16 CC_UP=0;
u16 CV_UP=0;
u16 CR_UP=0;
u16 CP_UP=0;
u8 CC_UP_FLAG=0;
u8 CV_UP_FLAG=0;
u8 CR_UP_FLAG=0;
u8 CP_UP_FLAG=0;
uint16_t ADD_HOT_SEND1=0;
uint16_t ADD_HOT_SEND=0;  //发送数据
uint16_t ADD_HOT_SEND_TEMP=0;  //发送数据
uint16_t ADD_HOT_Receive=0; //接收数据
uint16_t Voltage_ON=0;//当前电压
uint16_t Current_ON=0; //当前电流
uint16_t Temperature_ON=0; //当前温度
uint16_t P_ON=0;//当前功率
uint16_t Current_SET=0; //设定电流
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

	/***********************************ADC采样**********************************************/
//	for(i=0;i<5;i++)
//	{
//		HAL_ADC_Start(&ADC1_Handler);					 //开启ADC				
//		HAL_ADC_PollForConversion(&ADC1_Handler,0xffff); //等待ADC转换完成
//		adcBuf[i]=HAL_ADC_GetValue(&ADC1_Handler);		 //返回最近一次ADC1规则组的转换结果
//	}
	adcBuf[1]=Get_Adc_Average(ADC_CHANNEL_5,70,1);//电压
	adcBuf[2]=Get_Adc_Average_c(ADC_CHANNEL_10,70,2);//电流
	adcBuf[4]=Get_Adc_Average_T(ADC_CHANNEL_9,50,3);//温度
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
											//地址对应功能			屏幕地址码		
	usSRegInBuf[0] = Current*10;			//电流显示*10 			301
	usSRegInBuf[1] = adcBuf[0]*33/4096;		//断路器前电压显示*10 	302
	usSRegInBuf[2] = REVoltage*10; 			//断路器后电压显示*10 	303
	usSRegInBuf[3] = Temp*10;				//温度显示*10 			304
	usSRegInBuf[4] = Current;				//电流显示 				305
	usSRegInBuf[5] = adcBuf[0]*3.3/4096;	//断路器前电压显示 		306
	usSRegInBuf[6] = REVoltage;				//断路器后电压显示 		307
	usSRegInBuf[7] = Temp*10;				//温度显示 				308
	usSRegInBuf[8] = P_ON*10; 				//功率显示*10			309
	usSRegInBuf[9] = P_ON; 					//功率显示 				310
	HAL_ADC_Stop(&ADC1_Handler);
	/**********************************核对通讯值**********************************************/
	if(REVoltage<650) //限制值650.0
	{
		Voltage_ON=REVoltage;//电压比较输出			 
	}
	else if(REVoltage>650)
	{
		OUPUT_Y3=0;
	}
	if(Current<850)//限制值850.0
	{
		Current_ON=Current;//电流比较输出				 
	}
	else if(Current>850)
	{
		OUPUT_Y3=0;
	}
	if(Temp<400)//限制值400.0
	{
		Temperature_ON=Temp;//温度比较输出				 
	}
	else if(Temp>400)
	{
		OUPUT_Y3=0;
	}
	/*************************************CC模式输出*****************************************/
	if(INPUT_X2==CC)//CC开关开启
	{
		C_SET1=usSRegHoldBuf[1];
		mode=0x01;
		//核对通讯值				 
		if(C_SET1<850)//限制值850.0
		{
			Current_SET=C_SET1;//输出电流比较输出				 
		}			 
		//计算理论目标值				   
		ADD_HOT_SEND_TEMP=(((IGBT_R*Current_SET*IGBT_T)/Voltage_ON)/Totala_Sum);//占空比 （（（等效电阻*设定电流）/采集电压）/路数）*周期
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
//		ADD_HOT_SEND=ADD_HOT_SEND_TEMP;//设定理论计算值	 
	}
	/*************************************CV模式输出*****************************************/
	if(INPUT_X3==CV)//CV开关开启
	{
		V_SET1=usSRegHoldBuf[2];//设定电压
		mode=0x02;
		ADD_HOT_SEND_TEMP=(V_SET1*IGBT_T)/Voltage_ON;//目标K值  占空比=设定电压/采集电压=（等效电阻*(电流++))/采集电压
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
	/*************************************CW模式输出*****************************************/
	if(INPUT_X4==CW)//CW开关开启
	{
		P_SET1=usSRegHoldBuf[4];//设定功率
		mode=0x03;
		P_U_SET=sqrt((P_SET1*IGBT_R/Totala_Sum))*IGBT_T;
		ADD_HOT_SEND_TEMP=(P_U_SET/Voltage_ON);//目标K值  占空比=设定电压/采集电压=（设定功率/(电流++))/采集电压*1000
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
//		ADD_HOT_SEND=ADD_HOT_SEND_TEMP;//设定理论计算值	 
	}
	/*************************************CR模式输出*****************************************/
	if(INPUT_X5==CR)//CR开关开启
	{
		R_SET1=usSRegHoldBuf[3];//设定阻值
		mode=0x04;
		ADD_HOT_SEND_TEMP=(IGBT_R/R_SET1)/Totala_Sum*IGBT_T;//占空比=设定电压/采集电压=（等效电阻/设定阻值)/总路数*T	
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
		ADD_HOT_SEND=ADD_HOT_SEND_TEMP;//设定理论计算值
	}
	/*************************************SPI传输*****************************************/
	if(ADD_HOT_SEND>1001)//占空比不能大于100%
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
		/* 启动传输 */
		TEXT_FINISH2=SPI4_ReadWriteByte(ADD_HOT_SEND1);
		LED0=!LED0;
		TimingDelay1=2; //复位定时器
	} 
}

//Slave mode:HoldingRegister variables
//************************************************************************//
//**03 06 16功能码 
//**03 Read  Holding Registers(4x)
//**06 Write Single Registers
//**16 Write Multiple Registers
//**起始地址 400
//************************************************************************//

float adc_count(float value,u8 Mode)
{
	switch(Mode)
	{
		case 1:
			value=value*3.3/4096;			//4-20ma采集电压换算
			value=value/0.164;	//
			value=((value-4)*1000)/16;		//后电压换算
			return value;
		case 2:
			value=value*3.3/4096;			//4-20ma采集电流换算
			value=value/0.164;		//
			value=((value-4)*200)/16;		//后电流换算
			return value;
		case 3:
			value=value*3.3/4096;
			value=value-0.1;
			if(value>0.001)
			{
				value=value*(300/2.3);//温度换算
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
    
    sConfig.Channel=ch;                                   //通道
    sConfig.Rank=1;                                       //第1个序列，序列1
    sConfig.SamplingTime=ADC_SAMPLETIME_480CYCLES;        //采样时间
    sConfig.Offset=0;                 
    HAL_ADC_ConfigChannel(&ADC1_Handler,&sConfig);        //通道配置
	
    HAL_ADC_Start(&ADC1_Handler);                               //开启ADC
	
    HAL_ADC_PollForConversion(&ADC1_Handler,10);                //轮询转换
 
	return (u16)HAL_ADC_GetValue(&ADC1_Handler);	        //返回最近一次ADC1规则组的转换结果
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
	u8 temp_0;	//0的个数
	for(t=0;t<times;t++)
	{
		num[t]=Get_Adc(ch,rank);
		temp_val+=Get_Adc(ch,rank);
	}
	temp_0=num_0(num,70);				//电流数据不对
	temp_val=temp_val/(times-temp_0);	//注释掉这两行，去掉0的个数再除
	//temp_val=temp_val/times;			//解放这行
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
