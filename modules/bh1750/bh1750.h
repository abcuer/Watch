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

/* 全局总线实例（由 .c 文件定义）*/
extern iic_bus_t bh1750_bus;

/* 接口函数 */
void BH1750_Init(iic_bus_t *bus);
void BH1750_SendCmd(iic_bus_t *bus, uint8_t cmd);
uint16_t BH1750_ReadRaw(iic_bus_t *bus);
float BH1750_GetLux(iic_bus_t *bus);

#endif