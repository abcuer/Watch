// bh1750.c
#include "bh1750.h"
#include "bsp_delay.h"

iic_bus_t bh1750_bus = {
    .SDA_PORT = GPIOB,
    .SDA_PIN  = GPIO_PIN_11,
    .SCL_PORT = GPIOB,
    .SCL_PIN  = GPIO_PIN_10,
};

void BH1750_Init(iic_bus_t *bus)
{
    IIC_Init(bus);
    BH1750_SendCmd(bus, BH1750_POWER_ON);   
    BH1750_SendCmd(bus, BH1750_MODE_H1);   
    delay_ms(180);     // 必须等够！第一次测量需要 120~180ms
}

void BH1750_SendCmd(iic_bus_t *bus, uint8_t cmd)
{
    IIC_Start(bus);
    if (IIC_SendByte(bus, BH1750_ADDR_WRITE) ||
        IIC_SendByte(bus, cmd)) {
        IIC_Stop(bus);      
        return;
    }
    IIC_Stop(bus);
}

uint16_t BH1750_ReadRaw(iic_bus_t *bus)
{
    uint8_t buf[2];

    IIC_Start(bus);
    if (IIC_SendByte(bus, BH1750_ADDR_READ)) {
        IIC_Stop(bus);
        return 0xFFFF;
    }
    buf[0] = IIC_ReceiveByte(bus, 1);   
    buf[1] = IIC_ReceiveByte(bus, 0);   
    IIC_Stop(bus);
    return ((uint16_t)buf[0] << 8) | buf[1];
}

float BH1750_GetLux(iic_bus_t *bus)
{
    uint16_t raw = BH1750_ReadRaw(bus);
    return (raw == 0xFFFF) ? -1.0f : raw / 1.2f;
}