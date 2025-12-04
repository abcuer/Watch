#ifndef _DELAY_H
#define _DELAY_H
#include "sys.h"	  
#include "stdint.h"
#include "stm32f1xx_hal.h"   // 或 #include "stm32f10x.h" 视你的芯片与库而定

void delay_us(uint16_t us);
void delay_ms(uint16_t ms);
#endif

