#ifndef __SHT31_H
#define __SHT31_H

#include "stm32f1xx_hal.h"
#include "bsp_iic.h" 

#define SHT31_ADDR (0x44 << 1)   // 7bit地址左移一位变为8bit地址

void SHTInit(iic_bus_t *bus);
HAL_StatusTypeDef SHT31_ReadTempHum(iic_bus_t *bus, float *temperature, float *humidity);

extern iic_bus_t sht31_bus;

#endif
