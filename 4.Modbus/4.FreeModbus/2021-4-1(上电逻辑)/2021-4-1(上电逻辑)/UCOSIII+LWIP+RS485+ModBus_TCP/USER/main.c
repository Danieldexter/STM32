#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "lcd.h"
#include "rs485.h"
#include "sdram.h"
#include "lan8720.h"
#include "timer.h"
#include "pcf8574.h"
#include "usmart.h"
#include "malloc.h"
#include "lwip_comm.h"
#include "includes.h"
#include "lwipopts.h"
#include "mb.h"
#include "mbtcp.h"
#include "adc.h"
#include "gpio.h"
#include "spi.h"

//eMBTcpPoll����
//�������ȼ�
#define eMBTcpPoll_TASK_PRIO	7
//�����ջ��С
#define eMBTcpPoll_STK_SIZE	256
//������ƿ�
OS_TCB eMBTcpPollTaskTCB;
//�����ջ
CPU_STK	eMBTcpPoll_TASK_STK[eMBTcpPoll_STK_SIZE];
//������
void eMBTcpPoll_task(void *pdata);

//��LCD����ʾ��ַ��Ϣ����
//�������ȼ�
#define DISPLAY_TASK_PRIO	8
//�����ջ��С
#define DISPLAY_STK_SIZE	256
//������ƿ�
OS_TCB DisplayTaskTCB;
//�����ջ
CPU_STK	DISPLAY_TASK_STK[DISPLAY_STK_SIZE];
//������
void display_task(void *pdata);

//ADC����
//�������ȼ�
#define ADC_TASK_PRIO		9
//�����ջ��С
#define ADC_STK_SIZE		256
//������ƿ�
OS_TCB AdcTaskTCB;
//�����ջ
CPU_STK	ADC_TASK_STK[ADC_STK_SIZE];
//������
void adc_task(void *pdata);  

//RS485����
//�������ȼ�
#define eMBPoll_TASK_PRIO		10
//�����ջ��С
#define eMBPoll_STK_SIZE		256
//������ƿ�
OS_TCB eMBPollTaskTCB;
//�����ջ
CPU_STK	eMBPoll_TASK_STK[eMBPoll_STK_SIZE];
//������
void eMBPoll_task(void *pdata);  

//INPUT����
//�������ȼ�
#define INPUT_TASK_PRIO		11
//�����ջ��С
#define INPUT_STK_SIZE		256
//������ƿ�
OS_TCB INPUTTaskTCB;
//�����ջ
CPU_STK	INPUT_TASK_STK[INPUT_STK_SIZE];
//������
void input_task(void *pdata);  

//START����
//�������ȼ�
#define START_TASK_PRIO		12
//�����ջ��С
#define START_STK_SIZE		512
//�����ջ
OS_TCB StartTaskTCB;
//�����ջ
CPU_STK START_TASK_STK[START_STK_SIZE];
//������
void start_task(void *pdata); 

////////////////////////////////////////////////////////


//��LCD����ʾ��ַ��Ϣ
//mode:2 ��ʾDHCP��ȡ���ĵ�ַ
//	  ���� ��ʾ��̬��ַ
void show_address(u8 mode)
{
	u8 buf[30];
	if(mode==2)
	{
		sprintf((char*)buf,"DHCP IP :%d.%d.%d.%d",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);						//��ӡ��̬IP��ַ
		LCD_ShowString(30,130,210,16,16,buf); 
		sprintf((char*)buf,"DHCP GW :%d.%d.%d.%d",lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3]);	//��ӡ���ص�ַ
		LCD_ShowString(30,150,210,16,16,buf); 
		sprintf((char*)buf,"NET MASK:%d.%d.%d.%d",lwipdev.netmask[0],lwipdev.netmask[1],lwipdev.netmask[2],lwipdev.netmask[3]);	//��ӡ���������ַ
		LCD_ShowString(30,170,210,16,16,buf); 
	}
	else 
	{
		sprintf((char*)buf,"Static IP:%d.%d.%d.%d",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);						//��ӡ��̬IP��ַ
		LCD_ShowString(30,130,210,16,16,buf); 
		sprintf((char*)buf,"Static GW:%d.%d.%d.%d",lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3]);	//��ӡ���ص�ַ
		LCD_ShowString(30,150,210,16,16,buf); 
		sprintf((char*)buf,"NET MASK :%d.%d.%d.%d",lwipdev.netmask[0],lwipdev.netmask[1],lwipdev.netmask[2],lwipdev.netmask[3]);	//��ӡ���������ַ
		LCD_ShowString(30,170,210,16,16,buf); 
	}	
}

int main(void)
{   
	OS_ERR err;
	CPU_SR_ALLOC();
	
    Stm32_Clock_Init(360,25,2,8);   //����ʱ��,180Mhz   
    HAL_Init();                     //��ʼ��HAL��
    delay_init(180);                //��ʼ����ʱ����
    uart_init(115200);              //��ʼ��USART
    usmart_dev.init(90); 		    //��ʼ��USMART
	SPI4_Init();
	mGPIO_Init();	
    LED_Init();                     //��ʼ��LED 
    KEY_Init();                     //��ʼ������
	MY_ADC_Init();						
	RS485_Init(9600);		        //��ʼ��RS485
	OUPUT_Y10=1;					//��翪ʼ
	OUPUT_Y11=1;					//��翪ʼ
    my_mem_init(SRAMIN);		    //��ʼ���ڲ��ڴ��
	my_mem_init(SRAMEX);		    //��ʼ���ⲿ�ڴ��
	my_mem_init(SRAMCCM);		    //��ʼ��CCM�ڴ��
	POINT_COLOR = RED; 		        //��ɫ����
	OSInit(&err); 					//UCOSIII��ʼ�� 
	while(lwip_comm_init()) 		//lwip��ʼ��
	{
		printf("Lwip Init failed!\r\n");
		delay_ms(500);
		LCD_Fill(30,110,230,150,WHITE);
		delay_ms(500); 
		
	}
	printf("Lwip Init Success!\r\n");
	delay_ms(3000);
	OUPUT_Y5=0;	
	OS_CRITICAL_ENTER();//�����ٽ���x
	//������ʼ����
	OSTaskCreate((OS_TCB 	* )&StartTaskTCB,		//������ƿ�
				 (CPU_CHAR	* )"start task", 		//��������
                 (OS_TASK_PTR )start_task, 			//������
                 (void		* )0,					//���ݸ��������Ĳ���
                 (OS_PRIO	  )START_TASK_PRIO,     //�������ȼ�
                 (CPU_STK   * )&START_TASK_STK[0],	//�����ջ����ַ
                 (CPU_STK_SIZE)START_STK_SIZE/10,	//�����ջ�����λ
                 (CPU_STK_SIZE)START_STK_SIZE,		//�����ջ��С
                 (OS_MSG_QTY  )0,					//�����ڲ���Ϣ�����ܹ����յ������Ϣ��Ŀ,Ϊ0ʱ��ֹ������Ϣ
                 (OS_TICK	  )0,					//��ʹ��ʱ��Ƭ��תʱ��ʱ��Ƭ���ȣ�Ϊ0ʱΪĬ�ϳ��ȣ�
                 (void   	* )0,					//�û�����Ĵ洢��
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //����ѡ��
                 (OS_ERR 	* )&err);				//��Ÿú�������ʱ�ķ���ֵ
	OS_CRITICAL_EXIT();	//�˳��ٽ���	 
	OSStart(&err); 		//����UCOS
}

//start����
void start_task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	p_arg = p_arg;

	CPU_Init();
#if OS_CFG_STAT_TASK_EN > 0u
   OSStatTaskCPUUsageInit(&err);  	//ͳ������                
#endif
	
#ifdef CPU_CFG_INT_DIS_MEAS_EN		//���ʹ���˲����жϹر�ʱ��
    CPU_IntDisMeasMaxCurReset();	
#endif

#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //��ʹ��ʱ��Ƭ��ת��ʱ��
	 //ʹ��ʱ��Ƭ��ת���ȹ���,ʱ��Ƭ����Ϊ1��ϵͳʱ�ӽ��ģ���1*5=5ms
	OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);  
#endif		
	
#if LWIP_DHCP
	lwip_comm_dhcp_creat(); //����DHCP����
#endif
				
	OS_CRITICAL_ENTER();	//�����ٽ���
	//����eMBTcpPoll����
	OSTaskCreate((OS_TCB 	* )&eMBTcpPollTaskTCB,		
				 (CPU_CHAR	* )"eMBTcpPoll task", 		
                 (OS_TASK_PTR )eMBTcpPoll_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )eMBTcpPoll_TASK_PRIO,     
                 (CPU_STK   * )&eMBTcpPoll_TASK_STK[0],	
                 (CPU_STK_SIZE)eMBTcpPoll_STK_SIZE/10,	
                 (CPU_STK_SIZE)eMBTcpPoll_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);	
	
	//����ADC����
	OSTaskCreate((OS_TCB 	* )&AdcTaskTCB,		
				 (CPU_CHAR	* )"adc task", 		
                 (OS_TASK_PTR )adc_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )ADC_TASK_PRIO,     
                 (CPU_STK   * )&ADC_TASK_STK[0],	
                 (CPU_STK_SIZE)ADC_STK_SIZE/10,	
                 (CPU_STK_SIZE)ADC_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);	

	//����INPUT����
	OSTaskCreate((OS_TCB 	* )&INPUTTaskTCB,		
				 (CPU_CHAR	* )"INPUT task", 		
                 (OS_TASK_PTR )input_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )INPUT_TASK_PRIO,     
                 (CPU_STK   * )&INPUT_TASK_STK[0],	
                 (CPU_STK_SIZE)INPUT_STK_SIZE/10,	
                 (CPU_STK_SIZE)INPUT_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);	
	
	//����RS485����
	OSTaskCreate((OS_TCB 	* )&eMBPollTaskTCB,		
				 (CPU_CHAR	* )"eMBPoll task", 		
                 (OS_TASK_PTR )eMBPoll_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )eMBPoll_TASK_PRIO,     
                 (CPU_STK   * )&eMBPoll_TASK_STK[0],	
                 (CPU_STK_SIZE)eMBPoll_STK_SIZE/10,	
                 (CPU_STK_SIZE)eMBPoll_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);	
				 
	//������ʾ����
	OSTaskCreate((OS_TCB 	* )&DisplayTaskTCB,		
				 (CPU_CHAR	* )"display task", 		
                 (OS_TASK_PTR )display_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )DISPLAY_TASK_PRIO,     	
                 (CPU_STK   * )&DISPLAY_TASK_STK[0],	
                 (CPU_STK_SIZE)DISPLAY_STK_SIZE/10,	
                 (CPU_STK_SIZE)DISPLAY_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,				
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, 
                 (OS_ERR 	* )&err);
				 			 
	OS_TaskSuspend((OS_TCB*)&StartTaskTCB,&err);		//����ʼ����			 
	OS_CRITICAL_EXIT();	//�����ٽ���
}

//��ʾ��ַ����Ϣ
void display_task(void *pdata)
{
	OS_ERR err;
	while(1)
	{ 
#if LWIP_DHCP									//������DHCP��ʱ��
		if(lwipdev.dhcpstatus != 0) 			//����DHCP
		{
			show_address(lwipdev.dhcpstatus );	//��ʾ��ַ��Ϣ
			OS_TaskSuspend((OS_TCB*)&DisplayTaskTCB,&err);		//������������	
		}
#else
		show_address(0); 						//��ʾ��̬��ַ
		OSTaskSuspend(NULL,&err); 				//��ʾ���ַ��Ϣ�������������
#endif //LWIP_DHCP
		OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ500ms
	}
}


//eMBTcpPoll����
//************************************************************************//
//**03 06 16������ 
//**03 Read Holding Registers(4x)
//**06 Write Single Registers
//**16 Write Multiple Registers
//**��ʼ��ַ 400
//************************************************************************//
void eMBTcpPoll_task(void *pdata)
{
	OS_ERR err;
	eMBErrorCode    xStatus;
	eMBInitTCP(0);
	eMBEnableTcp();
	POINT_COLOR = BLUE; //��ɫ����
	printf("eMBPoll is Start!\r\n");
	while(1)
	{
		do
		{
		xStatus = eMBTcpPoll(  );
		OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ10ms
		}while( xStatus == MB_ENOERR );
		( void )eMBDisableTcp(  );
		( void )eMBCloseTcp(  );
		OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ10ms
 	}
}

//Slave mode:InputRegister variables
//************************************************************************//
//**04������ (3x)
//**04 Read Input Registers
//**��ʼ��ַ 300
//************************************************************************//
void adc_task(void *pdata)
{
	OS_ERR err;
	POINT_COLOR = BLUE; //��ɫ����
	printf("ADCPoll is Start!\r\n");
	while(1)
	{
		ADC_Poll();
		OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ500ms
 	}
}

//eMBPoll����
//************************************************************************//
//**03 06 16������ 
//**03 Read Holding Registers(4x)
//**06 Write Single Registers
//**16 Write Multiple Registers
//**��ʼ��ַ 400
//************************************************************************//
void eMBPoll_task(void *pdata)
{
	OS_ERR err;
	eMBInit(MB_RTU, 0x01, 3, 9600, MB_PAR_NONE);
	/* Enable the Modbus Protocol Stack. */
	eMBEnable();
	while(1)
	{
		(void)eMBPoll();
		OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ10ms			
 	}
}

void input_task(void *pdata)
{
	OS_ERR err;
	POINT_COLOR = BLUE; //��ɫ����
	printf("INPUT_Scan_Poll is Start!\r\n");
	while(1)
	{
		cheak_input2();
		cheak_input();
		input_scan_Poll();
		OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ500ms
	}
}




