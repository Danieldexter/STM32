#ifndef _LED_H
#define _LED_H
#include "sys.h"


#define LED0 PGout(0)   //LED0
#define LED1 PFout(13)	//LED1
#define LED2 PFout(14)	//LED2
#define LED3 PFout(15) 	//LED3

void LED_Init(void);
#endif
