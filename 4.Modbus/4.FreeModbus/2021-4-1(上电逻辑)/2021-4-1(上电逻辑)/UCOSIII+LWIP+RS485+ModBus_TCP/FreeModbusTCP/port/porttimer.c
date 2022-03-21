/*
 * FreeModbus Libary: BARE Port
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id$
 */

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"
#include "stm32f4xx_hal.h"
#include "rs485.h"
#include "os.h"
/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
extern TIM_HandleTypeDef TIM2_Handler;      //定时器句柄 
/* ----------------------- static functions ---------------------------------*/
//void prvvTIMERExpiredISR( void );
/* ----------------------- Start implementation -----------------------------*/
BOOL
xMBPortTimersInit( USHORT usTim1Timerout50us )
{
	TIM2_Init();
    return TRUE;
}


inline void
vMBPortTimersEnable(  )
{
    /* Enable the timer with the timeout passed to xMBPortTimersInit( ) */
	__HAL_TIM_CLEAR_IT(&TIM2_Handler,TIM_IT_UPDATE);
	__HAL_TIM_SetCounter(&TIM2_Handler,0);
  
	/* 在中断模式下启动定时器 */
	HAL_TIM_Base_Start_IT(&TIM2_Handler);
}

inline void
vMBPortTimersDisable(  )
{
    /* Disable any pending timers. */
	HAL_TIM_Base_Stop_IT(&TIM2_Handler);
  
	__HAL_TIM_SetCounter(&TIM2_Handler,0);
	__HAL_TIM_CLEAR_IT(&TIM2_Handler,TIM_IT_UPDATE); 
}

/* Create an ISR which is called whenever the timer has expired. This function
 * must then call pxMBPortCBTimerExpired( ) to notify the protocol stack that
 * the timer has expired.
 */
void prvvTIMERExpiredISR( void )
{
    ( void )pxMBPortCBTimerExpired(  );
}

//void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
//{
//	prvvTIMERExpiredISR();
//}

