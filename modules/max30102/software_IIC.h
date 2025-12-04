#ifndef __SOFTWARE_IIC_H
#define __SOFTWARE_IIC_H

#include "stdint.h"

void I2C_GPIO_Init(void);
void I2C_W_SCL(uint8_t BitValue);
void I2C_W_SDA(uint8_t BitValue);
uint8_t IIC_R_SDA(void);
void I2C_Start(void);
void I2C_Stop(void);
void I2C_SendByte(uint8_t Byte);
uint8_t I2C_ReceiveByte(void);
void I2C_SendAck(uint8_t AckBit);
uint8_t I2C_ReceiveAck(void);
void I2C_WriteReg(uint8_t slave_ID,uint8_t RegAddress, uint8_t Data);
uint8_t I2C_ReadReg(uint8_t slave_ID,uint8_t RegAddress);

#endif
