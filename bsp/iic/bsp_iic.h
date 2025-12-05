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
void IIC_SendNotAck(iic_bus_t *bus);

uint8_t IIC_SendByte(iic_bus_t *bus, uint8_t data);
uint8_t IIC_ReceiveByte(iic_bus_t *bus, uint8_t ack);

uint8_t IIC_Write_One_Byte(iic_bus_t *bus, uint8_t dev_addr, uint8_t reg, uint8_t data);
uint8_t IIC_Write_Multi_Byte(iic_bus_t *bus, uint8_t dev_addr, uint8_t reg, uint8_t *buf, uint8_t len);
uint8_t IIC_Read_One_Byte(iic_bus_t *bus, uint8_t dev_addr, uint8_t reg);
void IIC_Read_Multi_Byte(iic_bus_t *bus, uint8_t dev_addr, uint8_t reg, uint8_t *buf, uint8_t len);


#endif
