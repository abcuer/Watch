#ifndef __BSP_IIC_H
#define __BSP_IIC_H

#include "stdint.h"
#include "stm32f1xx_hal.h"

// 软件 I2C 总线结构体
typedef struct {
    GPIO_TypeDef* SDA_PORT;
    uint16_t      SDA_PIN;

    GPIO_TypeDef* SCL_PORT;
    uint16_t      SCL_PIN;
} iic_bus_t;

void IIC_Init(iic_bus_t *bus);
void IIC_Start(iic_bus_t *bus);
void IIC_Stop(iic_bus_t *bus);
uint8_t IIC_WaitAck(iic_bus_t *bus);
void IIC_SendAck(iic_bus_t *bus);
void IIC_SendNoAck(iic_bus_t *bus);

uint8_t IIC_WriteByte(iic_bus_t *bus, uint8_t data);
uint8_t IIC_ReceiveByte(iic_bus_t *bus, uint8_t ack);
uint8_t IIC_WriteReg(iic_bus_t *bus, uint8_t addr, uint8_t reg, uint8_t data);
uint8_t IIC_ReadReg(iic_bus_t *bus, uint8_t addr, uint8_t reg);
void IIC_ReadBytes(iic_bus_t *bus, uint8_t addr, uint8_t reg, uint8_t *buf, uint8_t len);


#endif
