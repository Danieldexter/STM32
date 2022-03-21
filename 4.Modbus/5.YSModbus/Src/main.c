/**
  ******************************************************************************
  * �ļ�����: main.c 
  * ��    ��: ӲʯǶ��ʽ�����Ŷ�
  * ��    ��: V1.0
  * ��д����: 2017-09-16
  * ��    ��: ModbusЭ��ӻ�RTUͨ��ģʽ
  ******************************************************************************
  * ˵����
  * ����������Ӳʯstm32������YS-F4Proʹ�á�
  * 
  * �Ա���
  * ��̳��http://www.ing10bbs.com
  * ��Ȩ��ӲʯǶ��ʽ�����Ŷ����У��������á�
  ******************************************************************************
  */
/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "usart/bsp_debug_usart.h"
#include "key/bsp_key.h"
#include "led/bsp_led.h"
#include "GeneralTIM/bsp_GeneralTIM.h"
#include "Modbus/bsp_MB_slave.h"
#include "key/bsp_key.h"
#include "RS485/bsp_usartx_RS485.h"
#include <stdlib.h>

/* ˽�����Ͷ��� --------------------------------------------------------------*/

/* ˽�к궨�� ----------------------------------------------------------------*/
#define MSG_ERR_FLAG  0xFFFF    // ���մ��� �ַ��䳬ʱ
#define MSG_IDLE      0x0000    // ����״̬
#define MSG_RXING     0x0001    // ���ڽ�������
#define MSG_COM       0x0002    // �������
#define MSG_INC       0x8000    // ����֡������(���ַ���Ŀ��м������1.5���ַ�ʱ��)
#define TIME_OVERRUN  100       // ���峬ʱʱ�� ms

/* ˽�б��� ------------------------------------------------------------------*/
__IO uint16_t Rx_MSG = MSG_IDLE;   // ���ձ���״̬
/* ��չ���� ------------------------------------------------------------------*/
extern REG_VALUE R_value;

/* ˽�к���ԭ�� --------------------------------------------------------------*/

/* ������ --------------------------------------------------------------------*/
/**
  * ��������: ϵͳʱ������
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
 
  __HAL_RCC_PWR_CLK_ENABLE();                                     //ʹ��PWRʱ��

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);  //���õ�ѹ�������ѹ����1

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;      // �ⲿ����8MHz
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;                        //��HSE 
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;                    //��PLL
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;            //PLLʱ��Դѡ��HSE
  RCC_OscInitStruct.PLL.PLLM = 8;                                 //8��ƵMHz
  RCC_OscInitStruct.PLL.PLLN = 336;                               //336��Ƶ
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;                     //2��Ƶ���õ�168MHz��ʱ��
  RCC_OscInitStruct.PLL.PLLQ = 7;                                 //USB/SDIO/������������ȵ���PLL��Ƶϵ��
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;       // ϵͳʱ�ӣ�168MHz
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;              // AHBʱ�ӣ� 168MHz
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;               // APB1ʱ�ӣ�42MHz
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;               // APB2ʱ�ӣ�84MHz
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);

  HAL_RCC_EnableCSS();                                            // ʹ��CSS���ܣ�����ʹ���ⲿ�����ڲ�ʱ��ԴΪ����
  
 	// HAL_RCC_GetHCLKFreq()/1000    1ms�ж�һ��
	// HAL_RCC_GetHCLKFreq()/100000	 10us�ж�һ��
	// HAL_RCC_GetHCLKFreq()/1000000 1us�ж�һ��
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);                // ���ò�����ϵͳ�δ�ʱ��
  /* ϵͳ�δ�ʱ��ʱ��Դ */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* ϵͳ�δ�ʱ���ж����ȼ����� */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/**
  * ��������: ��ʼ������
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
void InitPeri()
{
    /* ��λ�������裬��ʼ��Flash�ӿں�ϵͳ�δ�ʱ�� */
  HAL_Init();
  /* ����ϵͳʱ�� */
  SystemClock_Config();
    /* ��ʼ�����ڲ����ô����ж����ȼ� */
  MX_DEBUG_USART_Init(); 
  /* ��ʱ����ʼ�� */
  GENERAL_TIMx_Init();
  /* LED��ʼ�� */
  LED_GPIO_Init();
	/* KEY��ʼ�� */
	KEY_GPIO_Init();	
	/* Modbusʹ��RS485��ʼ�� */
	RS485_USARTx_Init();
  /* ʹ�ܶ�ʱ���ж� */
  __HAL_TIM_ENABLE_IT(&htimx,TIM_IT_CC1);
  __HAL_TIM_ENABLE_IT(&htimx,TIM_IT_UPDATE);
  /* �����ַ��䳬ʱʱ��1.5���ַ���ʱ�� */
  __HAL_TIM_SET_COMPARE(&htimx,TIM_CHANNEL_1,(uint16_t)OVERTIME_15CHAR);
  /* ����֡�䳬ʱʱ��3���ַ���ʱ�� */
  __HAL_TIM_SET_AUTORELOAD(&htimx,(uint16_t)OVERTIME_35CHAR); // ����֡�ڼ��ʱ��
}
/**
  * ��������: ����ڴ�
  * �������: buf:�ڴ�ռ��׵�ַ,Code:������
  * �� �� ֵ: ��
  * ˵    ��: ���ܲ�ͬ�Ĺ�����������ݲ�һ���ڴ�ռ�,
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
  * ��������: ������.
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
int main(void)
{
  uint16_t crc_check = 0;
  uint8_t Ex_code = 0,i = 0;

  /* ��ʼ������ */
  InitPeri();
  /* �����ڴ�ռ���Ϊ���ּĴ�����ַ,��Ӧ������03H��06H��10H */  
  PduData.PtrHoldingbase = (uint16_t*)malloc(sizeof(uint16_t)*0x125);
  FillBuf((uint8_t*)PduData.PtrHoldingbase,FUN_CODE_03H);
  
  printf(" -------Modbusͨ��Э������------ \n");
  printf("Ys-F4Pro  Modbus�ӻ�  RTUͨ��ģʽ\n");
  Rx_MSG = MSG_IDLE;
  /* ����ѭ�� */
  while (1)
  {
    /* ���յ�һ֡������,�Ի�����ȡ���� */
    if(Rx_MSG == MSG_COM)
    {
      for(i=0;i<8;i++)
      {
        printf("Rx_Buf[%d]=%d\n",i,Rx_Buf[i]);
      }    				
      /* �յ��Ǳ�����ַ����Ӧ���� */
      if((Rx_Buf[0] != MB_SLAVEADDR )&&(Rx_Buf[0] != MB_ALLSLAVEADDR))
      {
        Rx_MSG = MSG_IDLE;
        continue;
      }
      /* ��������֡ */
      MB_Parse_Data();

      /* CRC У����ȷ */
      crc_check = ( (Rx_Buf[RxCount-1]<<8) | Rx_Buf[RxCount-2] );
      if(crc_check == PduData._CRC) 
      {
        /* ��������֡��ִ�� */        
        Ex_code = MB_Analyze_Execute();
        /* �����쳣 */
        if(Ex_code !=EX_CODE_NONE)
        {
          MB_Exception_RSP(PduData.Code,Ex_code);
        }
        else
        {
          MB_RSP(PduData.Code);
        }
      }
      /* ���±��Ϊ����״̬ */
      Rx_MSG = MSG_IDLE;
    }
		if(KEY1_StateRead()==KEY_DOWN)
		{
			R_value.IN1=0x5263;
			R_value.D01=0x3;			
      LED3_TOGGLE;// ��LEDģ���ȡ��Ȧ״̬
      LED2_TOGGLE;// ��LEDģ���ȡ��Ȧ״̬
			LED1_TOGGLE;// ��LEDģ���ȡ��Ȧ״̬		  
		}		
  }
}
/** 
  * ��������: ���ڽ����жϻص�����
  * �������: ���ھ��
  * �� �� ֵ: ��
  * ˵    ��: ʹ��һ����ʱ���ıȽ��жϺ͸����ж���Ϊ���ճ�ʱ�ж�
  *           ֻҪ���յ����ݾͽ���ʱ����������0,�������Ƚ��жϵ�ʱ��
  *           ˵���Ѿ���ʱ1.5���ַ���ʱ��,�϶�Ϊ֡����,����Ǹ����ж�
  *           ����Ϊ�ǽ������
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if(huart == &husartx_rs485)
  {
    switch(Rx_MSG)
    {
      /* ���յ���һ���ַ�֮��ʼ��ʱ1.5/3.5���ַ���ʱ�� */
      case MSG_IDLE:
        Rx_MSG = MSG_RXING;
        RxCount = 0;
        HAL_TIM_Base_Start(&htimx);
        break;
      
      /* ������һ�ν��յ������Ѿ�����1.5���ַ���ʱ����,�϶�Ϊ����֡������ */
      case MSG_ERR_FLAG:
        printf("���յ�����֡������\n");
        Rx_MSG = MSG_INC; // ����֡������
      break;
    }
    
    /* ʹ�ܼ������� */
    Rx_Buf[RxCount] = tmp_Rx_Buf;
    RxCount++;
    __HAL_TIM_SET_COUNTER(&htimx,0); // �������ʱ��
    HAL_UART_Receive_IT(&husartx_rs485,(uint8_t*)&tmp_Rx_Buf,1);
  }
}
/** 
  * ��������: ��ʱ���Ƚ��жϻص�����
  * �������: ��ʱ�����
  * �� �� ֵ: ��
  * ˵    ��: ����Ѿ���ʱ1.5���ַ���ʱ��û�н��յ�����
  */
void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* ����ǵ�һ�η����Ƚ��ж�,���ݶ�Ϊ������ */
  if(Rx_MSG != MSG_INC)
    Rx_MSG = MSG_ERR_FLAG;
  
  /* ����ǵڶ��ν���Ƚ��ж�,���϶�Ϊ���Ĳ����� */
  else
    Rx_MSG = MSG_INC;
}
/** 
  * ��������: ��ʱ�������жϻص�����
  * �������: ��ʱ�����
  * �� �� ֵ: ��
  * ˵    ��: ��ʱ3.5���ַ���ʱ��û�н��յ�����,��Ϊ�ǿ���״̬ 
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* ����Ѿ�����˽��յ�������������֡,��������Ϊ������������֡ */
  if(Rx_MSG == MSG_INC)
  {
    Rx_MSG = MSG_INC;
  }
  /* �����������ʱ������� */
  else
  {
    Rx_MSG = MSG_COM;
  }
}
/******************* (C) COPYRIGHT 2015-2020 ӲʯǶ��ʽ�����Ŷ� *****END OF FILE****/
