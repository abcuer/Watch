#ifndef __MPU6050_I2C_H
#define __MPU6050_I2C_H

#include "stm32f1xx_hal.h"
#include "bsp_delay.h"      // delay_us()

// I2C GPIO 配置
#define MPU6050_IIC_GPIO_PORT       GPIOB
#define MPU6050_IIC_SCL_PIN         GPIO_PIN_10
#define MPU6050_IIC_SDA_PIN         GPIO_PIN_11

// 写 GPIO
#define MPU6050_IIC_SCL_HIGH()      HAL_GPIO_WritePin(MPU6050_IIC_GPIO_PORT, MPU6050_IIC_SCL_PIN, GPIO_PIN_SET)
#define MPU6050_IIC_SCL_LOW()       HAL_GPIO_WritePin(MPU6050_IIC_GPIO_PORT, MPU6050_IIC_SCL_PIN, GPIO_PIN_RESET)
#define MPU6050_IIC_SDA_HIGH()      HAL_GPIO_WritePin(MPU6050_IIC_GPIO_PORT, MPU6050_IIC_SDA_PIN, GPIO_PIN_SET)
#define MPU6050_IIC_SDA_LOW()       HAL_GPIO_WritePin(MPU6050_IIC_GPIO_PORT, MPU6050_IIC_SDA_PIN, GPIO_PIN_RESET)

// 读 GPIO
#define MPU6050_IIC_SDA_READ()      HAL_GPIO_ReadPin(MPU6050_IIC_GPIO_PORT, MPU6050_IIC_SDA_PIN)

#define MPU6050_IIC_delay_4us()     delay_us(4)

// ======================== 函数声明 ========================
void MPU6050_IIC_IO_Init(void);
void MPU6050_IIC_SDA_OUT(void);
void MPU6050_IIC_SDA_IN(void);

void MPU6050_IIC_Start(void);
void MPU6050_IIC_Stop(void);

void MPU6050_IIC_Send_Byte(uint8_t data);
uint8_t MPU6050_IIC_Read_Byte(uint8_t ack);

uint8_t MPU6050_IIC_Wait_Ack(void);
void MPU6050_IIC_Send_Ack(uint8_t ack);

#endif
