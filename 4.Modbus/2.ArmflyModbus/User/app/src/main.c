/*
*********************************************************************************************************
*
*	模块名称 : 主程序模块。
*	文件名称 : main.c
*	版    本 : V1.4
*	说    明 : RS485 MODBUS从站例程（使用的是串口3）。
*              本例程主要讲解MODBUS协议从站的命令处理方法,包含了常用的命令。
*   实验内容：
*              1. 接好硬件,(1)串口1(打印实验数据)  (2)485接口(收发命令)
*              2. MODBUS调试助手发送送命令,串口打印出结果。
*              3. 命令：
*						01H:	  485地址   命令  	寄存器地址   寄存器数量	   校验码
*							发送 	01  	 01   	  01 01   	   00 05       AC 35
*								  485地址  寄存器地址  结果       校验码
*							接收    01 	    01 01       02 		   D0 49 
*							说明: 结果中的每一位表示LED的状态，例如： 02 = 0000 0010 表示 LED2亮 其他灭。
*
*						05H:
*							发送	01 05 01 01 00 01 5C 36 
*							接收	01 05 01 01 00 01 5C 36 
*   					
*	修改记录 :
*		版本号   日期       作者       说明
*		v1.4    2015-11-28  armfly     首发
*
*	Copyright (C), 2015-2016, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"			/* 底层硬件驱动 */
#include "modbus_slave.h"
#include "main.h"

/* 定义例程名和例程发布日期 */
#define EXAMPLE_NAME	"V4-RS485 MODBUS从站例程"
#define EXAMPLE_DATE	"2016-01-11"
#define DEMO_VER		"1.0"

/* 仅允许本文件内调用的函数声明 */
static void PrintfLogo(void);
static void DispMenu(void);
static void SetLed(void);

PRINT_MODS_T g_tPrint;

/*
*********************************************************************************************************
*	函 数 名: main
*	功能说明: c程序入口
*	形    参：无
*	返 回 值: 错误代码(无需处理)
*********************************************************************************************************
*/
int main(void)
{
	MSG_T ucMsg;

	bsp_Init();							/* 硬件初始化 */
	PrintfLogo();						/* 打印例程信息到串口1 */
	DispMenu();							/* 打印寄存器的值 */
	
	/* 进入主程序循环体 */
	while (1)
	{
		bsp_Idle();						/* 调用MODS_Poll() */
		
		if (bsp_GetMsg(&ucMsg))
		{
			switch (ucMsg.MsgCode)
			{
				case MSG_MODS_05H:		/* 打印 发送的命令 和 应答的命令  刷新LED状态 */
					DispMenu();
					SetLed();			/* 设置LED亮灭(处理05H指令) */
					break;
				
				default:
					DispMenu();
					break;
			}
		}
	}
}

/*
*********************************************************************************************************
*	函 数 名: SetLed
*	功能说明: 设置LED亮灭(处理05H指令)
*	形    参：无
*	返 回 值: 错误代码(无需处理)
*********************************************************************************************************
*/
static void SetLed(void)
{
	if (g_tVar.D01 == 1) bsp_LedOn(1);else bsp_LedOff(1);	/* LED1 */
	if (g_tVar.D02 == 1) bsp_LedOn(2);else bsp_LedOff(2);	/* LED2 */
	if (g_tVar.D03 == 1) bsp_LedOn(3);else bsp_LedOff(3);	/* LED3 */
	if (g_tVar.D04 == 1) bsp_LedOn(4);else bsp_LedOff(4);	/* LED4 */
}

/*
*********************************************************************************************************
*	函 数 名: PrintfLogo
*	功能说明: 打印例程名称和例程发布日期, 接上串口线后，打开PC机的超级终端软件可以观察结果
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void PrintfLogo(void)
{
	printf("\n\r");
	printf("*************************************************************\n\r");
	printf("* 例程名称   : %s\r\n", EXAMPLE_NAME);	/* 打印例程名称 */
	printf("* 例程版本   : %s\r\n", DEMO_VER);		/* 打印例程版本 */
	printf("* 发布日期   : %s\r\n", EXAMPLE_DATE);	/* 打印例程日期 */

	/* 打印ST固件库版本，这3个定义宏在stm32f10x.h文件中 */
	printf("* 固件库版本 : V%d.%d.%d (STM32F10x_StdPeriph_Driver)\r\n", __STM32F10X_STDPERIPH_VERSION_MAIN,
			__STM32F10X_STDPERIPH_VERSION_SUB1,__STM32F10X_STDPERIPH_VERSION_SUB2);
	printf("* \n\r");	/* 打印一行空格 */
	printf("* QQ    : 1295744630 \r\n");
	printf("* 旺旺  : armfly\r\n");
	printf("* Email : armfly@qq.com \r\n");
	printf("* 淘宝店: armfly.taobao.com\r\n");
	printf("* Copyright www.armfly.com 安富莱电子\r\n");
	printf("*************************************************************\n\r");
}

/*
*********************************************************************************************************
*	函 数 名: DispMenu
*	功能说明: 打印例程结果
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void DispMenu(void)
{	
	uint8_t i;
	
	printf("\n\r");
	printf("\33[K");						/* 清除从光标到行尾的内容 */ 
	printf(" P01 = %d\r\n", g_tVar.P01);	/* 保持寄存器1 */
	printf("\33[K");						/* 清除从光标到行尾的内容 */ 
	printf(" P02 = %d\r\n", g_tVar.P02);	/* 保持寄存器2 */
	printf("\33[K");						/* 清除从光标到行尾的内容 */ 
	printf(" A01 = %d\r\n", g_tVar.A01);	/* adc值 */
	printf("\n\r");
	printf("\33[K");						/* 清除从光标到行尾的内容 */ 
	printf(" 接收的命令 : 0x");				/* 打印接收命令 */
	for (i = 0; i < g_tPrint.Rxlen; i++)
	{
		printf(" %02X", g_tPrint.RxBuf[i]);
	}
	printf("\n\r");
	printf("\33[K");						/* 清除从光标到行尾的内容 */ 
	printf(" 发送的命令 : 0x");				/* 打印发送命令 */
	for (i = 0; i < g_tPrint.Txlen; i++)
	{
		printf(" %02X", g_tPrint.TxBuf[i]);
	}
	printf("\n\r");
	printf("\33[7A");						/* 光标上移6行 */
	
	g_tVar.A01 = AdcValue;					/* 得到输入模拟量 */
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
