#ifndef __BH1750_H
#define __BH1750_H

#include <stdint.h>

/********************************************
 * BH1750 引脚配置（按你的需求）
 ********************************************/
#define BH1750_GPIO_PORT      GPIOB
#define BH1750_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOB_CLK_ENABLE()

#define BH1750_SCL_GPIO_PIN   GPIO_PIN_10
#define BH1750_SDA_GPIO_PIN   GPIO_PIN_11

/********************************************
 * BH1750 设备地址
 ********************************************/
#define BHAddWrite    0x46   // 写地址
#define BHAddRead     0x47   // 读地址

/********************************************
 * 命令
 ********************************************/
#define BHPowDown     0x00
#define BHPowOn       0x01
#define BHReset       0x07

#define BHModeH1      0x10   // 高分辨率：1lx，120ms
#define BHModeH2      0x11   // 高分辨率2：0.5lx，120ms
#define BHModeL       0x13   // 低分辨率：4lx，16ms

/********************************************
 * 函数声明
 ********************************************/
void BH1750_Init(void);
void bh_data_send(uint8_t command);
uint16_t bh_data_read(void);
float BH1750_GetLux(void);

/* 模拟 IIC */
void BH1750_IIC_Start(void);
void BH1750_IIC_Stop(void);
void BH1750_IIC_Send_Byte(uint8_t txd);
uint8_t BH1750_IIC_Read_Byte(uint8_t ack);
uint8_t BH1750_IIC_Wait_Ack(void);
void BH1750_IIC_Ack(void);
void BH1750_IIC_NAck(void);

void SDA_OUT(void);
void SDA_IN(void);

#endif
