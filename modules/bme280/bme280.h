#ifndef __BME280_H
#define __BME280_H

#include "stdint.h"
#include "bsp_iic.h"


#define BME280_ADDR 0x76   // 注意：软件 IIC 使用 7bit 地址，不需要左移

typedef struct
{
    uint16_t dig_T1;
    int16_t  dig_T2;
    int16_t  dig_T3;

    uint16_t dig_P1;
    int16_t  dig_P2;
    int16_t  dig_P3;
    int16_t  dig_P4;
    int16_t  dig_P5;
    int16_t  dig_P6;
    int16_t  dig_P7;
    int16_t  dig_P8;
    int16_t  dig_P9;

    uint8_t  dig_H1;
    int16_t  dig_H2;
    uint8_t  dig_H3;
    int16_t  dig_H4;
    int16_t  dig_H5;
    int8_t   dig_H6;

    int32_t  t_fine;

    iic_bus_t *bus;
} BME280_t;

void BME280_Init(BME280_t *dev, iic_bus_t *bus);
void BME280_ReadData(BME280_t *dev, float *T, float *P, float *H);

#endif
