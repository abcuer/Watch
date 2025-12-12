#ifndef __BH1750_H
#define __BH1750_H

#include "stdint.h"
#include "bsp_iic.h"
#include "bsp_delay.h"

/* BH1750 I2C 地址 */
#define BH1750_ADDR_WRITE   0x46
#define BH1750_ADDR_READ    0x47

/* 命令 */
#define BH1750_POWER_DOWN   0x00
#define BH1750_POWER_ON     0x01
#define BH1750_RESET        0x07
#define BH1750_MODE_H1      0x10
#define BH1750_MODE_H2      0x11
#define BH1750_MODE_L       0x13

void BH1750_Init(void);
void BH1750_SendCmd(uint8_t cmd);
uint16_t BH1750_ReadRaw(void);
float BH1750_GetLux(void);

typedef struct
{
    float lux;
}BH1750_t;

#endif