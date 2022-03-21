#include "gpio.h"
#include "user_mb_app.h"
#include "led.h"
#include "delay.h"
#include "os.h"
#include "adc.h"

OS_TMR 	tmr1;		//��ʱ��1
static void CreatTmr1(void);
static void tmr1_callback(void *p_tmr, void *p_arg); 	//��ʱ��1�ص�����

u8 INPUT_X0 =0;//��ѹ
u8 INPUT_X1 =0;//���
u8 INPUT_X2 =0;//CC����
u8 INPUT_X3 =0;//CV����
u8 INPUT_X4 =0;//CW����
u8 INPUT_X5 =0;//CR����
u8 INPUT_X6 =0;
u8 INPUT_X7 =0;

u8 INPUT_X0_FLAG =0;
u8 INPUT_X1_FLAG =0;
u8 INPUT_X2_FLAG =0;
u8 INPUT_X3_FLAG =0;
u8 INPUT_X4_FLAG =0;
u8 INPUT_X5_FLAG =0;
u8 INPUT_X6_FLAG =0;
u8 INPUT_X7_FLAG =0;

u8 INPUT_X10_FLAG =0;
u8 INPUT_X11_FLAG =0;
u8 INPUT_X12_FLAG =0;
u8 INPUT_X13_FLAG =0;
u8 INPUT_X14_FLAG =0;
u8 INPUT_X15_FLAG =0;
u8 INPUT_X16_FLAG =0;
u8 INPUT_X17_FLAG =0;

static u8 tmr1_sec=0;
static u8 tmr1_min=0;
static u8 tmr1_hour=0;

extern uint16_t ADD_HOT_SEND;
extern uint16_t mode;


void mGPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_Initure;
	__HAL_RCC_GPIOA_CLK_ENABLE();           //����GPIOAʱ��
	__HAL_RCC_GPIOB_CLK_ENABLE();           //����GPIOBʱ��
	__HAL_RCC_GPIOC_CLK_ENABLE();           //����GPIOCʱ��
	__HAL_RCC_GPIOD_CLK_ENABLE();           //����GPIODʱ��
	__HAL_RCC_GPIOH_CLK_ENABLE();           //����GPIOHʱ��
	__HAL_RCC_GPIOI_CLK_ENABLE();           //����GPIOHʱ��


	/*--------------------------------------------------------------------------------------*/
	
//	/*��Ļ���ⰴ��X0~X7*/
//	
//	GPIO_Initure.Pin=GPIO_PIN_8;			//PA8
//    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //�������
//    GPIO_Initure.Pull=GPIO_PULLUP;          //����
//    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //����
//    HAL_GPIO_Init(GPIOA,&GPIO_Initure);		//��ʼ��GPIOA
//	
//	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8 ,GPIO_PIN_RESET);//PA8��0  
//	
//	GPIO_Initure.Pin=GPIO_PIN_10			//PD10
//					|GPIO_PIN_11			//PD11
//					|GPIO_PIN_12		    //PD12
//					|GPIO_PIN_13;		    //PD13
//    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //�������
//    GPIO_Initure.Pull=GPIO_PULLUP;          //����
//    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //����
//    HAL_GPIO_Init(GPIOD,&GPIO_Initure);		//��ʼ��GPIOD
//	
//	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_10,GPIO_PIN_RESET);//PD10��0  
//	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_11,GPIO_PIN_RESET);//PD11��0  
//	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_12,GPIO_PIN_RESET);//PD12��0  
//	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_13,GPIO_PIN_RESET);//PD13��0  
//	
//	GPIO_Initure.Pin=GPIO_PIN_3;	    	//PH3
//    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //�������
//    GPIO_Initure.Pull=GPIO_PULLUP;          //����
//    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //����
//    HAL_GPIO_Init(GPIOH,&GPIO_Initure);		//��ʼ��GPIOB
//	
//	HAL_GPIO_WritePin(GPIOH,GPIO_PIN_3,GPIO_PIN_RESET);//PH3��0
	
//	GPIO_Initure.Pin=GPIO_PIN_0				//PI0
//					|GPIO_PIN_1;	    	//PI1
//    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //�������
//    GPIO_Initure.Pull=GPIO_PULLUP;          //����
//    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //����
//    HAL_GPIO_Init(GPIOI,&GPIO_Initure);		//��ʼ��GPIOI
//	
//	HAL_GPIO_WritePin(GPIOI,GPIO_PIN_0,GPIO_PIN_RESET);//PI0��0
//	HAL_GPIO_WritePin(GPIOI,GPIO_PIN_1,GPIO_PIN_RESET);//PI1��0

	/* ����X0~X7 */
	
	GPIO_Initure.Pin=GPIO_PIN_4				//PB4
					|GPIO_PIN_5				//PB5
					|GPIO_PIN_6		     	//PB6
					|GPIO_PIN_7;		    //PB7
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //�������
    GPIO_Initure.Pull=GPIO_PULLUP;          //����
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //����
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);		//��ʼ��GPIOB
	
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4 ,GPIO_PIN_RESET);//PB4��0  
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5 ,GPIO_PIN_RESET);//PB5��0  
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6 ,GPIO_PIN_RESET);//PB6��0  
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7 ,GPIO_PIN_RESET);//PB7��0  

	GPIO_Initure.Pin=GPIO_PIN_4				//PI4
					|GPIO_PIN_5				//PI5
					|GPIO_PIN_6				//PI6
					|GPIO_PIN_7;			//PI7	
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //�������
    GPIO_Initure.Pull=GPIO_PULLUP;          //����
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //����
    HAL_GPIO_Init(GPIOI,&GPIO_Initure);		//��ʼ��GPIOI
	
	HAL_GPIO_WritePin(GPIOI,GPIO_PIN_4 ,GPIO_PIN_RESET);//PI4��0  
	HAL_GPIO_WritePin(GPIOI,GPIO_PIN_5 ,GPIO_PIN_RESET);//PI5��0  
	HAL_GPIO_WritePin(GPIOI,GPIO_PIN_6 ,GPIO_PIN_RESET);//PI6��0  
	HAL_GPIO_WritePin(GPIOI,GPIO_PIN_7 ,GPIO_PIN_RESET);//IC7��0  
	
	
	/*--------------------------------------------------------------------------------------*/
	
	/* ���Y0~Y17 */
	
	GPIO_Initure.Pin=GPIO_PIN_15;			//PA15
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //�������
    GPIO_Initure.Pull=GPIO_PULLUP;          //����
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //����
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);		//��ʼ��GPIOA
	
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_15,GPIO_PIN_RESET);//PA15��1 

	GPIO_Initure.Pin=GPIO_PIN_14			//PB14
					|GPIO_PIN_15;			//PB15
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //�������
    GPIO_Initure.Pull=GPIO_PULLUP;          //����
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //����
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);		//��ʼ��GPIOB
	
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_14 ,GPIO_PIN_SET);//PB14��1 
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_15 ,GPIO_PIN_SET);//PB14��1 
	
	GPIO_Initure.Pin=GPIO_PIN_10			//PC10
					|GPIO_PIN_11			//PC11
					|GPIO_PIN_12;			//PC12
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //�������
    GPIO_Initure.Pull=GPIO_PULLUP;          //����
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //����
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);		//��ʼ��GPIOC
	
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_10 ,GPIO_PIN_RESET);//PC10��1 
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_11 ,GPIO_PIN_SET);//PC11��1 
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_12 ,GPIO_PIN_SET);//PC12��1 

	GPIO_Initure.Pin=GPIO_PIN_0				//PD0
					|GPIO_PIN_1				//PD1
					|GPIO_PIN_2				//PD2
					|GPIO_PIN_4				//PD4
					|GPIO_PIN_8				//PD8
					|GPIO_PIN_9;			//PD9
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //�������
    GPIO_Initure.Pull=GPIO_PULLUP;          //����
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //����
    HAL_GPIO_Init(GPIOD,&GPIO_Initure);		//��ʼ��GPIOD
	
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_0,GPIO_PIN_SET);//PD8 ��1
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_1,GPIO_PIN_SET);//PD9 ��1 
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_SET);//PD10��1
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_4,GPIO_PIN_SET);//PD11��1 
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_8,GPIO_PIN_SET);//PD12��1 
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_9,GPIO_PIN_SET);//PD13��1 

	GPIO_Initure.Pin=GPIO_PIN_8				//PH8
					|GPIO_PIN_9				//PH9
					|GPIO_PIN_10			//PH10
					|GPIO_PIN_11;			//PH11
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //�������
    GPIO_Initure.Pull=GPIO_PULLUP;          //����
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //����
    HAL_GPIO_Init(GPIOH,&GPIO_Initure);		//��ʼ��GPIOH

	HAL_GPIO_WritePin(GPIOH,GPIO_PIN_8,GPIO_PIN_SET);//PH8��1
	HAL_GPIO_WritePin(GPIOH,GPIO_PIN_9,GPIO_PIN_SET);//PH9��1 
	HAL_GPIO_WritePin(GPIOH,GPIO_PIN_10,GPIO_PIN_SET);//PH10��1 
	HAL_GPIO_WritePin(GPIOH,GPIO_PIN_11,GPIO_PIN_SET);//PH11��1



/*--------------------------------------------------------------------------------------*/
}

u8 OUTSIDE_INPUT_Scan(u8 mode2)
{
	static u8 input_up=1;
	if(mode2==1)	input_up=1;
	if(input_up&&(INPUT_X10==0||INPUT_X11==0||INPUT_X12==0||INPUT_X13==0||INPUT_X14==0
				||INPUT_X15==0||INPUT_X16==0||INPUT_X17==0))
	{
		input_up=0;
			 if(INPUT_X10==0)return INPUT_X10_IN;
		else if(INPUT_X11==0)return INPUT_X11_IN;
		else if(INPUT_X12==0)return INPUT_X12_IN;
		else if(INPUT_X13==0)return INPUT_X13_IN;
		else if(INPUT_X14==0)return INPUT_X14_IN;
		else if(INPUT_X15==0)return INPUT_X15_IN;
		else if(INPUT_X16==0)return INPUT_X16_IN;
		else if(INPUT_X17==0)return INPUT_X17_IN;
	}
	else if(INPUT_X10==1||INPUT_X11==1||INPUT_X12==1||INPUT_X13==1||INPUT_X14==1
		  ||INPUT_X15==1||INPUT_X16==1||INPUT_X17==1)
	{
		input_up=1;
		if(INPUT_X10_FLAG==1)	//X10������X0�����ж����ر��ж����
		{
			INPUT_X10_FLAG=0;
			LED1=1;
		}
		
	}
	return 0;
}

void cheak_input2(void)
{
	switch(OUTSIDE_INPUT_Scan(0))
	{
		/*--------------------------*/
		case INPUT_X10_IN:
			if(INPUT_X10_FLAG==0)	//X10������X0�����ж����
			{
				INPUT_X10_FLAG=1;
				LED1=0;		
				__set_FAULTMASK(1); //�����λ
				NVIC_SystemReset();
			}
		break;
	}
}

extern USHORT   usSRegInBuf[];//����Ĵ���
u8 INPUT_Scan(u8 mode1)
{
	OS_ERR err;
	//���X0���루���
	if(INPUT_X0==1&&INPUT_X0_FLAG==0)
	{
		INPUT_X0_FLAG = 1;
		printf("X0 ��翪��\r\n");
		OUPUT_Y10=1;
		OUPUT_Y11=1;
		LED0=!LED0;
		return INPUT_X0_IN;
	}
	else if(INPUT_X0==0&&INPUT_X0_FLAG==1)
	{
		INPUT_X0_FLAG = 0;
		OUPUT_Y10=0;	
		OUPUT_Y11=0;
		printf("X0 ���ر�\r\n");
	}
	//���X1����
	if(INPUT_X1==1&&INPUT_X1_FLAG==0)
	{
		INPUT_X1_FLAG = 1;
		return INPUT_X1_IN;
	}
	else if(INPUT_X1==0&&INPUT_X1_FLAG==1)
	{
		INPUT_X1_FLAG = 0;
	}
	//���X2���루CC����
	if(INPUT_X2==1&&INPUT_X2_FLAG==0)
	{
		if(INPUT_X3==0&&INPUT_X4==0&&INPUT_X5==0)//��ֹͬʱ����������
		{
			OUPUT_Y5=1;
			OUPUT_Y4=0;
			INPUT_X2_FLAG = 1;
			printf("X2 CC��\r\n");
			return INPUT_X2_IN;
		}
		else INPUT_X2=0;
	}
	else if(INPUT_X2==0&&INPUT_X2_FLAG==1)
	{
		CC_UP=0;
		CC_UP_FLAG=0;
		OUPUT_Y4=1;
		OUPUT_Y5=0;
		INPUT_X2_FLAG = 0;
		usSRegInBuf[10]=0;
		usSRegInBuf[11]=0;
		usSRegInBuf[12]=0;
		OSTmrDel(&tmr1,&err);
		tmr1_sec=0;
		tmr1_min=0;
		tmr1_hour=0;
		
		ADD_HOT_SEND_TEMP=0;
		ADD_HOT_SEND=0;
		mode=0;
		
		OUPUT_Y1=1;
		delay_ms(5000);
		OUPUT_Y2=1;
		delay_ms(200);
		OUPUT_Y0=1;
		
		printf("X2 CC�ر�\r\n");
	}
	//���X3����
	if(INPUT_X3==1&&INPUT_X3_FLAG==0)
	{
		if(INPUT_X2==0&&INPUT_X4==0&&INPUT_X5==0)
		{
			OUPUT_Y5=1;
			OUPUT_Y4=0;
			INPUT_X3_FLAG = 1;
			return INPUT_X3_IN;
			printf("X3 CV��\r\n");
		}
		else INPUT_X3=0;
	}
	else if(INPUT_X3==0&&INPUT_X3_FLAG==1)
	{
		OUPUT_Y4=1;
		OUPUT_Y5=0;
		INPUT_X3_FLAG = 0;
		usSRegInBuf[10]=0;
		usSRegInBuf[11]=0;
		usSRegInBuf[12]=0;
		OSTmrDel(&tmr1,&err);
		tmr1_sec=0;
		tmr1_min=0;
		tmr1_hour=0;
		
		ADD_HOT_SEND_TEMP=0;
		ADD_HOT_SEND=0;
		mode=0;
		
		OUPUT_Y1=1;
		delay_ms(5000);	//�ϵ����
		OUPUT_Y2=1;
		delay_ms(200);
		OUPUT_Y0=1;
		
		printf("X3 CV�ر�\r\n");
	}
	//���X4����
	if(INPUT_X4==1&&INPUT_X4_FLAG==0)
	{
		if(INPUT_X2==0&&INPUT_X3==0&&INPUT_X5==0)
		{
			OUPUT_Y5=1;
			OUPUT_Y4=0;
			INPUT_X4_FLAG = 1;
			return INPUT_X4_IN;
			printf("X4 CW��\r\n");
		}
		else INPUT_X4=0;
	}
	else if(INPUT_X4==0&&INPUT_X4_FLAG==1)
	{
		OUPUT_Y4=1;
		OUPUT_Y5=0;
		INPUT_X4_FLAG = 0;
		usSRegInBuf[10]=0;
		usSRegInBuf[11]=0;
		usSRegInBuf[12]=0;
		OSTmrDel(&tmr1,&err);
		tmr1_sec=0;
		tmr1_min=0;
		tmr1_hour=0;
		
		ADD_HOT_SEND_TEMP=0;
		ADD_HOT_SEND=0;
		mode=0;
		
		OUPUT_Y1=1;
		delay_ms(5000);
		OUPUT_Y2=1;
		delay_ms(200);
		OUPUT_Y0=1;
		
		printf("X4 CW�ر�\r\n");
	}
	//���X5����
	if(INPUT_X5==1&&INPUT_X5_FLAG==0)
	{
		if(INPUT_X2==0&&INPUT_X3==0&&INPUT_X4==0)
		{
			OUPUT_Y5=1;
			OUPUT_Y4=0;
			INPUT_X5_FLAG = 1;
			return INPUT_X5_IN;
			printf("X5 CR��\r\n");
		}
		else INPUT_X5=0;
	}
	else if(INPUT_X5==0&&INPUT_X5_FLAG==1)
	{
		OUPUT_Y4=1;
		OUPUT_Y5=0;
		INPUT_X5_FLAG = 0;
		usSRegInBuf[10]=0;
		usSRegInBuf[11]=0;
		usSRegInBuf[12]=0;
		OSTmrDel(&tmr1,&err);
		tmr1_sec=0;
		tmr1_min=0;
		tmr1_hour=0;
		
		ADD_HOT_SEND_TEMP=0;
		ADD_HOT_SEND=0;
		mode=0;
		
		OUPUT_Y1=1;
		delay_ms(5000);
		OUPUT_Y2=1;
		delay_ms(200);
		OUPUT_Y0=1;
		printf("X5 CR�ر�\r\n");
	}
	//���X6����
//	if(INPUT_X6==1&&INPUT_X6_FLAG==0)
//	{
//		if(INPUT_X2==0&&INPUT_X3==0&&INPUT_X4==0)
//		{
//			INPUT_X6_FLAG = 1;
//			return INPUT_X6_IN;
//			printf("X6 CR��\r\n");
//		}
//		else INPUT_X6=0;
//	}
//	else if(INPUT_X6==0&&INPUT_X6_FLAG==1)
//	{
//		INPUT_X6_FLAG = 0;
//		usSRegInBuf[10]=0;
//		usSRegInBuf[11]=0;
//		usSRegInBuf[12]=0;
//		OSTmrDel(&tmr1,&err);
//		tmr1_sec=0;
//		tmr1_min=0;
//		tmr1_hour=0;
//		OUPUT_Y1=1;
//		delay_ms(100);
//		OUPUT_Y0=1;
//		delay_ms(100);
//		OUPUT_Y2=1;
//		printf("X6 CR�ر�\r\n");
//	}
//	//���X7����
//	if(INPUT_X7==1&&INPUT_X7_FLAG==0)
//	{
//		INPUT_X7_FLAG = 1;
//		return INPUT_X7_IN;
//	}
//	else if(INPUT_X7==0&&INPUT_X7_FLAG==1)
//	{
//		INPUT_X7_FLAG = 0;
//	}
	return 0;// ����û������
}
extern USHORT usSRegHoldBuf[];
u8 iii=0;
void cheak_input(void)
{
	OS_ERR err;
	switch(INPUT_Scan(0))
	{
		/*--------------------------*/
		case INPUT_X0_IN:			//X0������
			printf("X0����");	
							
		break;
		/*--------------------------*/
		case INPUT_X1_IN:			//X1������
				printf("X1����");
//				OUPUT_Y12=0; //����ӿ�
//				LED0=!LED0;
						
		break;
		/*--------------------------*/
		case INPUT_X2_IN:			//X2������(CC���
				printf("X2����");
				CreatTmr1();	//������ʱ��1
				OUPUT_Y0=0;
				delay_ms(500);
				OUPUT_Y1=0;
				delay_ms(500);
				OUPUT_Y2=0;
			
		break;
		/*--------------------------*/
		case INPUT_X3_IN:		//X3������(CV���
				printf("X3����");
				CreatTmr1();	//������ʱ��1
				OUPUT_Y0=0;
				delay_ms(500);
				OUPUT_Y1=0;
				delay_ms(500);
				OUPUT_Y2=0;
					
		break;
		/*--------------------------*/
		case INPUT_X4_IN:		//X4������(CW���
				printf("X4����");
				CreatTmr1();	//������ʱ��1
				OUPUT_Y0=0;
				delay_ms(500);
				OUPUT_Y1=0;
				delay_ms(500);
				OUPUT_Y2=0;
			
		break;
		/*--------------------------*/
		case INPUT_X5_IN:		//X5������(CR���
				printf("X5����");
				CreatTmr1();	//������ʱ��1
				OUPUT_Y0=0;
				delay_ms(500);
				OUPUT_Y1=0;
				delay_ms(500);
				OUPUT_Y2=0;
		break;		
		/*--------------------------*/
//		case INPUT_X6_IN:		//X6������
//				printf("X6����");
//			
////			
////		break;		
//		/*--------------------------*/
//		case INPUT_X7_IN:		//X7������
//			printf("X7����");
//			LED0=!LED0;
//			
//		break;		
	}
}
//led��ѯ���� 
//************************************************************************//
//**01 05 15������
//**01 Read Coils��0x��  ��ȡ
//**05 Write Single Coil д��
//**15 Write Multiple Coils  д������
//**��ʼ��ַ 000
//************************************************************************//
extern UCHAR    ucSCoilBuf[]  ;
extern UCHAR    ucSDiscInBuf[]  ;
void input_scan_Poll(void)
{
    u8 SW_Status = ucSCoilBuf[0];
	if(SW_Status & 0x01) {INPUT_X0  = 1;} else {INPUT_X0  = 0;}//0x01 ��ѹ
	if(SW_Status & 0x02) {INPUT_X1  = 1;} else {INPUT_X1  = 0;}//0x02 ���
	if(SW_Status & 0x04) {INPUT_X2  = 1;} else {INPUT_X2  = 0;}//0x03 CC���
	if(SW_Status & 0x08) {INPUT_X3  = 1;} else {INPUT_X3  = 0;}//0x04 CV���
	if(SW_Status & 0x10) {INPUT_X4  = 1;} else {INPUT_X4  = 0;}//0x05 CW���
	if(SW_Status & 0x20) {INPUT_X5  = 1;} else {INPUT_X5  = 0;}//0x06 CR���
	if(SW_Status & 0x40) {INPUT_X6  = 1;} else {INPUT_X6  = 0;}//0x07
	if(SW_Status & 0x80) {INPUT_X7  = 1;} else {INPUT_X7  = 0;}//0x08
	u8 Key_Status = 0x00;
	INPUT_X10_IN? (Key_Status &=~ 0x01):(Key_Status |= 0x01);
    INPUT_X11_IN? (Key_Status &=~ 0x02):(Key_Status |= 0x02); 
    INPUT_X12_IN? (Key_Status &=~ 0x04):(Key_Status |= 0x04);
	ucSDiscInBuf[0] = Key_Status;
}


static void CreatTmr1(void)
{
	OS_ERR err;
	OS_TMR Tmr1;
	OSTmrCreate((OS_TMR		*)&tmr1,		//��ʱ��1
                (CPU_CHAR	*)"tmr1",		//��ʱ������
                (OS_TICK	 )0,			//20*10=200ms
                (OS_TICK	 )100,          //100*10=1000ms
                (OS_OPT		 )OS_OPT_TMR_PERIODIC, //����ģʽ
                (OS_TMR_CALLBACK_PTR)tmr1_callback,//��ʱ��1�ص�����
                (void	    *)0,			//����Ϊ0
                (OS_ERR	    *)&err);		//���صĴ�����	
				
	if(err == OS_ERR_NONE)
	{
		OSTmrStart((OS_TMR        *)    &tmr1,/*���������ʱ��*/
				   (OS_ERR        *)    &err);
	}
	else
	{
		#if DEBUG_ENABLE
		PRINTF_Dbg("create tmr2 failed..\n");
		#endif
	}
}

//��ʱ��1�Ļص�����
extern USHORT   usSRegInBuf[];//����Ĵ���
void tmr1_callback(void *p_tmr, void *p_arg)
{
	
	tmr1_sec++;		//��ʱ��1ִ�д�����1
	if(tmr1_sec==60)
	{
		tmr1_sec=0;
		tmr1_min++;
	}
	if(tmr1_min==60)
	{
		tmr1_min=0;
		tmr1_hour++;
	}
	usSRegInBuf[10]=tmr1_hour;
	usSRegInBuf[11]=tmr1_min;
	usSRegInBuf[12]=tmr1_sec;
	printf("%d:%d:%d\r\n",tmr1_hour,tmr1_min,tmr1_sec);
}
