#ifndef __CRC_H__
#define __CRC_H__
#include "stm32f1xx_hal.h"


uint8_t bbc (uint8_t *buf, uint8_t len); //bbcУ�� 
uint16_t   N_CRC16(uint8_t *updata,uint16_t len);
uint8_t lrc (uint8_t *buf, uint8_t len); //lrcУ��


#endif
