#include "bsp_tim.h"

uint16_t usTimerT35_50us = 0;

void TIM4_IRQHandler(void)
{
    if (__HAL_TIM_GET_FLAG(&htim4, TIM_FLAG_UPDATE) != RESET)
    {
        switch(RevSta)
        {
            case RevIDLE:
			{
				HAL_TIM_Base_Stop_IT(&htim4);
			    __HAL_TIM_SetCounter(&htim4,0);
			}
            break;
            case RevRXNE:
            {
				RevSta = RevIDLE;
				if(++rxhead >= 10)
					rxhead = 0;
                HAL_TIM_Base_Stop_IT(&htim4);
			    __HAL_TIM_SetCounter(&htim4,0);
			}
			break;
	    }
		__HAL_TIM_CLEAR_IT(&htim4, TIM_IT_UPDATE);
    }
}

