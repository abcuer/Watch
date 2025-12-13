#include "bh1750.h"

// 定义总线实例
iic_bus_t bh1750_bus = {
    .IIC_SDA_PORT = GPIOB,
    .IIC_SDA_PIN = GPIO_PIN_11,
    .IIC_SCL_PORT = GPIOB,
    .IIC_SCL_PIN = GPIO_PIN_10
};

void BH1750_Init(void)
{
    IICInit(&bh1750_bus);
    BH1750_SendCmd(BH1750_POWER_ON);   
    BH1750_SendCmd(BH1750_MODE_H1);   
    // 等待第一次测量完成
    delay_ms(180);     // 必须等够！第一次测量需要 120~180ms
}

void BH1750_SendCmd(uint8_t cmd)
{
    IICStart(&bh1750_bus);
    IICSendByte(&bh1750_bus, BH1750_ADDR_WRITE);
    if (IICWaitAck(&bh1750_bus) != SUCCESS) {
        IICStop(&bh1750_bus);
        return;  // 发送失败
    }
    IICSendByte(&bh1750_bus, cmd);
    IICWaitAck(&bh1750_bus);
    IICStop(&bh1750_bus);
}

uint16_t BH1750_ReadRaw(void)
{
    uint8_t buf[2] = {0, 0};
    
    IICStart(&bh1750_bus);
    IICSendByte(&bh1750_bus, BH1750_ADDR_READ);
    if (IICWaitAck(&bh1750_bus) != SUCCESS) {
        IICStop(&bh1750_bus);
        return 0xFFFF;  // 读取失败
    }
    buf[0] = IICReceiveByte(&bh1750_bus);
    IICSendAck(&bh1750_bus);                
    buf[1] = IICReceiveByte(&bh1750_bus);
    IICSendNotAck(&bh1750_bus);             
    IICStop(&bh1750_bus);
    return ((uint16_t)buf[0] << 8) | buf[1];
}

float BH1750_GetLux(void)
{
    uint16_t raw = BH1750_ReadRaw();
    if (raw == 0xFFFF) {
        return -1.0f;  // 读取失败
    }
    return (float)raw / 1.2f;
}

void BH1750_Reset(void)
{
    BH1750_SendCmd(BH1750_RESET);
    delay_ms(1);
}

void BH1750_PowerDown(void)
{
    BH1750_SendCmd(BH1750_POWER_DOWN);
}

void BH1750_ChangeMode(uint8_t mode)
{
    BH1750_SendCmd(mode);
    switch (mode) {
        case BH1750_MODE_H1:
            delay_ms(180); 
            break;
        case BH1750_MODE_H2:
            delay_ms(180);  
            break;
        case BH1750_MODE_L:
            delay_ms(24);   
            break;
        default:
            delay_ms(180);  
    }
}