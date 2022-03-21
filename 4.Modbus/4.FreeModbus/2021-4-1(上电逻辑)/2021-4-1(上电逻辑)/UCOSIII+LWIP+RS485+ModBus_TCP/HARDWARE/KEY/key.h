#ifndef _KEY_H
#define _KEY_H
#include "sys.h"

#define KEY0        PHin(4) //KEY0°´¼üPA0

#define KEY0_PRES 	1

void KEY_Init(void);
u8 KEY_Scan(u8 mode);
#endif
