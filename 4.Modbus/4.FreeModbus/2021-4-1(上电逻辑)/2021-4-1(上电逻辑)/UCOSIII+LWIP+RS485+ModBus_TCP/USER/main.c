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

//eMBTcpPoll任务
//任务优先级
#define eMBTcpPoll_TASK_PRIO	7
//任务堆栈大小
#define eMBTcpPoll_STK_SIZE	256
//任务控制块
OS_TCB eMBTcpPollTaskTCB;
//任务堆栈
CPU_STK	eMBTcpPoll_TASK_STK[eMBTcpPoll_STK_SIZE];
//任务函数
void eMBTcpPoll_task(void *pdata);

//在LCD上显示地址信息任务
//任务优先级
#define DISPLAY_TASK_PRIO	8
//任务堆栈大小
#define DISPLAY_STK_SIZE	256
//任务控制块
OS_TCB DisplayTaskTCB;
//任务堆栈
CPU_STK	DISPLAY_TASK_STK[DISPLAY_STK_SIZE];
//任务函数
void display_task(void *pdata);

//ADC任务
//任务优先级
#define ADC_TASK_PRIO		9
//任务堆栈大小
#define ADC_STK_SIZE		256
//任务控制块
OS_TCB AdcTaskTCB;
//任务堆栈
CPU_STK	ADC_TASK_STK[ADC_STK_SIZE];
//任务函数
void adc_task(void *pdata);  

//RS485任务
//任务优先级
#define eMBPoll_TASK_PRIO		10
//任务堆栈大小
#define eMBPoll_STK_SIZE		256
//任务控制块
OS_TCB eMBPollTaskTCB;
//任务堆栈
CPU_STK	eMBPoll_TASK_STK[eMBPoll_STK_SIZE];
//任务函数
void eMBPoll_task(void *pdata);  

//INPUT任务
//任务优先级
#define INPUT_TASK_PRIO		11
//任务堆栈大小
#define INPUT_STK_SIZE		256
//任务控制块
OS_TCB INPUTTaskTCB;
//任务堆栈
CPU_STK	INPUT_TASK_STK[INPUT_STK_SIZE];
//任务函数
void input_task(void *pdata);  

//START任务
//任务优先级
#define START_TASK_PRIO		12
//任务堆栈大小
#define START_STK_SIZE		512
//任务堆栈
OS_TCB StartTaskTCB;
//任务堆栈
CPU_STK START_TASK_STK[START_STK_SIZE];
//任务函数
void start_task(void *pdata); 

////////////////////////////////////////////////////////


//在LCD上显示地址信息
//mode:2 显示DHCP获取到的地址
//	  其他 显示静态地址
void show_address(u8 mode)
{
	u8 buf[30];
	if(mode==2)
	{
		sprintf((char*)buf,"DHCP IP :%d.%d.%d.%d",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);						//打印动态IP地址
		LCD_ShowString(30,130,210,16,16,buf); 
		sprintf((char*)buf,"DHCP GW :%d.%d.%d.%d",lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3]);	//打印网关地址
		LCD_ShowString(30,150,210,16,16,buf); 
		sprintf((char*)buf,"NET MASK:%d.%d.%d.%d",lwipdev.netmask[0],lwipdev.netmask[1],lwipdev.netmask[2],lwipdev.netmask[3]);	//打印子网掩码地址
		LCD_ShowString(30,170,210,16,16,buf); 
	}
	else 
	{
		sprintf((char*)buf,"Static IP:%d.%d.%d.%d",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);						//打印动态IP地址
		LCD_ShowString(30,130,210,16,16,buf); 
		sprintf((char*)buf,"Static GW:%d.%d.%d.%d",lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3]);	//打印网关地址
		LCD_ShowString(30,150,210,16,16,buf); 
		sprintf((char*)buf,"NET MASK :%d.%d.%d.%d",lwipdev.netmask[0],lwipdev.netmask[1],lwipdev.netmask[2],lwipdev.netmask[3]);	//打印子网掩码地址
		LCD_ShowString(30,170,210,16,16,buf); 
	}	
}

int main(void)
{   
	OS_ERR err;
	CPU_SR_ALLOC();
	
    Stm32_Clock_Init(360,25,2,8);   //设置时钟,180Mhz   
    HAL_Init();                     //初始化HAL库
    delay_init(180);                //初始化延时函数
    uart_init(115200);              //初始化USART
    usmart_dev.init(90); 		    //初始化USMART
	SPI4_Init();
	mGPIO_Init();	
    LED_Init();                     //初始化LED 
    KEY_Init();                     //初始化按键
	MY_ADC_Init();						
	RS485_Init(9600);		        //初始化RS485
	OUPUT_Y10=1;					//充电开始
	OUPUT_Y11=1;					//充电开始
    my_mem_init(SRAMIN);		    //初始化内部内存池
	my_mem_init(SRAMEX);		    //初始化外部内存池
	my_mem_init(SRAMCCM);		    //初始化CCM内存池
	POINT_COLOR = RED; 		        //红色字体
	OSInit(&err); 					//UCOSIII初始化 
	while(lwip_comm_init()) 		//lwip初始化
	{
		printf("Lwip Init failed!\r\n");
		delay_ms(500);
		LCD_Fill(30,110,230,150,WHITE);
		delay_ms(500); 
		
	}
	printf("Lwip Init Success!\r\n");
	delay_ms(3000);
	OUPUT_Y5=0;	
	OS_CRITICAL_ENTER();//进入临界区x
	//创建开始任务
	OSTaskCreate((OS_TCB 	* )&StartTaskTCB,		//任务控制块
				 (CPU_CHAR	* )"start task", 		//任务名字
                 (OS_TASK_PTR )start_task, 			//任务函数
                 (void		* )0,					//传递给任务函数的参数
                 (OS_PRIO	  )START_TASK_PRIO,     //任务优先级
                 (CPU_STK   * )&START_TASK_STK[0],	//任务堆栈基地址
                 (CPU_STK_SIZE)START_STK_SIZE/10,	//任务堆栈深度限位
                 (CPU_STK_SIZE)START_STK_SIZE,		//任务堆栈大小
                 (OS_MSG_QTY  )0,					//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
                 (OS_TICK	  )0,					//当使能时间片轮转时的时间片长度，为0时为默认长度，
                 (void   	* )0,					//用户补充的存储区
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //任务选项
                 (OS_ERR 	* )&err);				//存放该函数错误时的返回值
	OS_CRITICAL_EXIT();	//退出临界区	 
	OSStart(&err); 		//开启UCOS
}

//start任务
void start_task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	p_arg = p_arg;

	CPU_Init();
#if OS_CFG_STAT_TASK_EN > 0u
   OSStatTaskCPUUsageInit(&err);  	//统计任务                
#endif
	
#ifdef CPU_CFG_INT_DIS_MEAS_EN		//如果使能了测量中断关闭时间
    CPU_IntDisMeasMaxCurReset();	
#endif

#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //当使用时间片轮转的时候
	 //使能时间片轮转调度功能,时间片长度为1个系统时钟节拍，既1*5=5ms
	OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);  
#endif		
	
#if LWIP_DHCP
	lwip_comm_dhcp_creat(); //创建DHCP任务
#endif
				
	OS_CRITICAL_ENTER();	//进入临界区
	//创建eMBTcpPoll任务
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
	
	//创建ADC任务
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

	//创建INPUT任务
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
	
	//创建RS485任务
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
				 
	//创建显示任务
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
				 			 
	OS_TaskSuspend((OS_TCB*)&StartTaskTCB,&err);		//挂起开始任务			 
	OS_CRITICAL_EXIT();	//进入临界区
}

//显示地址等信息
void display_task(void *pdata)
{
	OS_ERR err;
	while(1)
	{ 
#if LWIP_DHCP									//当开启DHCP的时候
		if(lwipdev.dhcpstatus != 0) 			//开启DHCP
		{
			show_address(lwipdev.dhcpstatus );	//显示地址信息
			OS_TaskSuspend((OS_TCB*)&DisplayTaskTCB,&err);		//挂起自身任务	
		}
#else
		show_address(0); 						//显示静态地址
		OSTaskSuspend(NULL,&err); 				//显示完地址信息后挂起自身任务
#endif //LWIP_DHCP
		OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_STRICT,&err); //延时500ms
	}
}


//eMBTcpPoll任务
//************************************************************************//
//**03 06 16功能码 
//**03 Read Holding Registers(4x)
//**06 Write Single Registers
//**16 Write Multiple Registers
//**起始地址 400
//************************************************************************//
void eMBTcpPoll_task(void *pdata)
{
	OS_ERR err;
	eMBErrorCode    xStatus;
	eMBInitTCP(0);
	eMBEnableTcp();
	POINT_COLOR = BLUE; //蓝色字体
	printf("eMBPoll is Start!\r\n");
	while(1)
	{
		do
		{
		xStatus = eMBTcpPoll(  );
		OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_HMSM_STRICT,&err); //延时10ms
		}while( xStatus == MB_ENOERR );
		( void )eMBDisableTcp(  );
		( void )eMBCloseTcp(  );
		OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_HMSM_STRICT,&err); //延时10ms
 	}
}

//Slave mode:InputRegister variables
//************************************************************************//
//**04功能码 (3x)
//**04 Read Input Registers
//**起始地址 300
//************************************************************************//
void adc_task(void *pdata)
{
	OS_ERR err;
	POINT_COLOR = BLUE; //蓝色字体
	printf("ADCPoll is Start!\r\n");
	while(1)
	{
		ADC_Poll();
		OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_STRICT,&err); //延时500ms
 	}
}

//eMBPoll任务
//************************************************************************//
//**03 06 16功能码 
//**03 Read Holding Registers(4x)
//**06 Write Single Registers
//**16 Write Multiple Registers
//**起始地址 400
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
		OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_HMSM_STRICT,&err); //延时10ms			
 	}
}

void input_task(void *pdata)
{
	OS_ERR err;
	POINT_COLOR = BLUE; //蓝色字体
	printf("INPUT_Scan_Poll is Start!\r\n");
	while(1)
	{
		cheak_input2();
		cheak_input();
		input_scan_Poll();
		OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_HMSM_STRICT,&err); //延时500ms
	}
}




