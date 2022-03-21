#ifndef __SPI_H
#define __SPI_H
#include "sys.h"


extern SPI_HandleTypeDef SPI4_Handler;  //SPI¾ä±ú

void SPI4_Init(void);
void SPI4_SetSpeed(u8 SPI_BaudRatePrescaler);
uint16_t SPI4_ReadWriteByte(uint16_t TxData);
#endif
