#ifndef __SHT31_H
#define __SHT31_H

#include "stm32f1xx_hal.h"
#include "bsp_iic.h" 

#define SHT31_ADDR 0x44   

void SHTInit(void);
HAL_StatusTypeDef SHT31_ReadTempHum(float *temperature, float *humidity);

typedef struct
{
    float temp;
    float hum;
} SHT31_t;

#endif
