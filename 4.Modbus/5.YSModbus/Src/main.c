/**
  ******************************************************************************
  * 文件名程: main.c 
  * 作    者: 硬石嵌入式开发团队
  * 版    本: V1.0
  * 编写日期: 2017-09-16
  * 功    能: Modbus协议从机RTU通信模式
  ******************************************************************************
  * 说明：
  * 本例程配套硬石stm32开发板YS-F4Pro使用。
  * 
  * 淘宝：
  * 论坛：http://www.ing10bbs.com
  * 版权归硬石嵌入式开发团队所有，请勿商用。
  ******************************************************************************
  */
/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "usart/bsp_debug_usart.h"
#include "key/bsp_key.h"
#include "led/bsp_led.h"
#include "GeneralTIM/bsp_GeneralTIM.h"
#include "Modbus/bsp_MB_slave.h"
#include "key/bsp_key.h"
#include "RS485/bsp_usartx_RS485.h"
#include <stdlib.h>

/* 私有类型定义 --------------------------------------------------------------*/

/* 私有宏定义 ----------------------------------------------------------------*/
#define MSG_ERR_FLAG  0xFFFF    // 接收错误 字符间超时
#define MSG_IDLE      0x0000    // 空闲状态
#define MSG_RXING     0x0001    // 正在接收数据
#define MSG_COM       0x0002    // 接收完成
#define MSG_INC       0x8000    // 数据帧不完整(两字符间的空闲间隔大于1.5个字符时间)
#define TIME_OVERRUN  100       // 定义超时时间 ms

/* 私有变量 ------------------------------------------------------------------*/
__IO uint16_t Rx_MSG = MSG_IDLE;   // 接收报文状态
/* 扩展变量 ------------------------------------------------------------------*/
extern REG_VALUE R_value;

/* 私有函数原形 --------------------------------------------------------------*/

/* 函数体 --------------------------------------------------------------------*/
/**
  * 函数功能: 系统时钟配置
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
 
  __HAL_RCC_PWR_CLK_ENABLE();                                     //使能PWR时钟

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);  //设置调压器输出电压级别1

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;      // 外部晶振，8MHz
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;                        //打开HSE 
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;                    //打开PLL
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;            //PLL时钟源选择HSE
  RCC_OscInitStruct.PLL.PLLM = 8;                                 //8分频MHz
  RCC_OscInitStruct.PLL.PLLN = 336;                               //336倍频
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;                     //2分频，得到168MHz主时钟
  RCC_OscInitStruct.PLL.PLLQ = 7;                                 //USB/SDIO/随机数产生器等的主PLL分频系数
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;       // 系统时钟：168MHz
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;              // AHB时钟： 168MHz
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;               // APB1时钟：42MHz
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;               // APB2时钟：84MHz
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);

  HAL_RCC_EnableCSS();                                            // 使能CSS功能，优先使用外部晶振，内部时钟源为备用
  
 	// HAL_RCC_GetHCLKFreq()/1000    1ms中断一次
	// HAL_RCC_GetHCLKFreq()/100000	 10us中断一次
	// HAL_RCC_GetHCLKFreq()/1000000 1us中断一次
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);                // 配置并启动系统滴答定时器
  /* 系统滴答定时器时钟源 */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* 系统滴答定时器中断优先级配置 */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/**
  * 函数功能: 初始化外设
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */
void InitPeri()
{
    /* 复位所有外设，初始化Flash接口和系统滴答定时器 */
  HAL_Init();
  /* 配置系统时钟 */
  SystemClock_Config();
    /* 初始化串口并配置串口中断优先级 */
  MX_DEBUG_USART_Init(); 
  /* 定时器初始化 */
  GENERAL_TIMx_Init();
  /* LED初始化 */
  LED_GPIO_Init();
	/* KEY初始化 */
	KEY_GPIO_Init();	
	/* Modbus使用RS485初始化 */
	RS485_USARTx_Init();
  /* 使能定时器中断 */
  __HAL_TIM_ENABLE_IT(&htimx,TIM_IT_CC1);
  __HAL_TIM_ENABLE_IT(&htimx,TIM_IT_UPDATE);
  /* 设置字符间超时时间1.5个字符的时间 */
  __HAL_TIM_SET_COMPARE(&htimx,TIM_CHANNEL_1,(uint16_t)OVERTIME_15CHAR);
  /* 设置帧间超时时间3个字符的时间 */
  __HAL_TIM_SET_AUTORELOAD(&htimx,(uint16_t)OVERTIME_35CHAR); // 设置帧内间隔时间
}
/**
  * 函数功能: 填充内存
  * 输入参数: buf:内存空间首地址,Code:功能码
  * 返 回 值: 无
  * 说    明: 功能不同的功能码填充内容不一的内存空间,
  */
void FillBuf(uint8_t* buf,uint8_t Code)
{
  uint16_t i = 0;
  uint16_t j = 1;
  switch(Code)
  {
    case FUN_CODE_03H:
    case FUN_CODE_06H:
    case FUN_CODE_10H:
      j = 0x000F;
      for(i= 0;i<0x250;i++)
      buf[i] = j++;
    break;
  }
}
/**
  * 函数功能: 主函数.
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */
int main(void)
{
  uint16_t crc_check = 0;
  uint8_t Ex_code = 0,i = 0;

  /* 初始化外设 */
  InitPeri();
  /* 申请内存空间作为保持寄存器地址,对应功能码03H、06H和10H */  
  PduData.PtrHoldingbase = (uint16_t*)malloc(sizeof(uint16_t)*0x125);
  FillBuf((uint8_t*)PduData.PtrHoldingbase,FUN_CODE_03H);
  
  printf(" -------Modbus通信协议例程------ \n");
  printf("Ys-F4Pro  Modbus从机  RTU通信模式\n");
  Rx_MSG = MSG_IDLE;
  /* 无限循环 */
  while (1)
  {
    /* 接收到一帧的数据,对缓存提取数据 */
    if(Rx_MSG == MSG_COM)
    {
      for(i=0;i<8;i++)
      {
        printf("Rx_Buf[%d]=%d\n",i,Rx_Buf[i]);
      }    				
      /* 收到非本机地址的响应请求 */
      if((Rx_Buf[0] != MB_SLAVEADDR )&&(Rx_Buf[0] != MB_ALLSLAVEADDR))
      {
        Rx_MSG = MSG_IDLE;
        continue;
      }
      /* 解析数据帧 */
      MB_Parse_Data();

      /* CRC 校验正确 */
      crc_check = ( (Rx_Buf[RxCount-1]<<8) | Rx_Buf[RxCount-2] );
      if(crc_check == PduData._CRC) 
      {
        /* 分析数据帧并执行 */        
        Ex_code = MB_Analyze_Execute();
        /* 出现异常 */
        if(Ex_code !=EX_CODE_NONE)
        {
          MB_Exception_RSP(PduData.Code,Ex_code);
        }
        else
        {
          MB_RSP(PduData.Code);
        }
      }
      /* 重新标记为空闲状态 */
      Rx_MSG = MSG_IDLE;
    }
		if(KEY1_StateRead()==KEY_DOWN)
		{
			R_value.IN1=0x5263;
			R_value.D01=0x3;			
      LED3_TOGGLE;// 以LED模拟读取线圈状态
      LED2_TOGGLE;// 以LED模拟读取线圈状态
			LED1_TOGGLE;// 以LED模拟读取线圈状态		  
		}		
  }
}
/** 
  * 函数功能: 串口接收中断回调函数
  * 输入参数: 串口句柄
  * 返 回 值: 无
  * 说    明: 使用一个定时器的比较中断和更新中断作为接收超时判断
  *           只要接收到数据就将定时器计数器清0,当发生比较中断的时候
  *           说明已经超时1.5个字符的时间,认定为帧错误,如果是更新中断
  *           则认为是接受完成
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if(huart == &husartx_rs485)
  {
    switch(Rx_MSG)
    {
      /* 接收到第一个字符之后开始计时1.5/3.5个字符的时间 */
      case MSG_IDLE:
        Rx_MSG = MSG_RXING;
        RxCount = 0;
        HAL_TIM_Base_Start(&htimx);
        break;
      
      /* 距离上一次接收到数据已经超过1.5个字符的时间间隔,认定为数据帧不完整 */
      case MSG_ERR_FLAG:
        printf("接收的数据帧不完整\n");
        Rx_MSG = MSG_INC; // 数据帧不完整
      break;
    }
    
    /* 使能继续接收 */
    Rx_Buf[RxCount] = tmp_Rx_Buf;
    RxCount++;
    __HAL_TIM_SET_COUNTER(&htimx,0); // 重设计数时间
    HAL_UART_Receive_IT(&husartx_rs485,(uint8_t*)&tmp_Rx_Buf,1);
  }
}
/** 
  * 函数功能: 定时器比较中断回调函数
  * 输入参数: 定时器句柄
  * 返 回 值: 无
  * 说    明: 标记已经超时1.5个字符的时间没有接收到数据
  */
void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* 如果是第一次发生比较中断,则暂定为错误标记 */
  if(Rx_MSG != MSG_INC)
    Rx_MSG = MSG_ERR_FLAG;
  
  /* 如果是第二次进入比较中断,则认定为报文不完整 */
  else
    Rx_MSG = MSG_INC;
}
/** 
  * 函数功能: 定时器更新中断回调函数
  * 输入参数: 定时器句柄
  * 返 回 值: 无
  * 说    明: 超时3.5个字符的时间没有接收到数据,认为是空闲状态 
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* 如果已经标记了接收到不完整的数据帧,则继续标记为不完整的数据帧 */
  if(Rx_MSG == MSG_INC)
  {
    Rx_MSG = MSG_INC;
  }
  /* 在正常情况下时接收完成 */
  else
  {
    Rx_MSG = MSG_COM;
  }
}
/******************* (C) COPYRIGHT 2015-2020 硬石嵌入式开发团队 *****END OF FILE****/
