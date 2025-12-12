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
    // 初始化I2C总线
    IICInit(&bh1750_bus);
    
    // 发送上电命令
    BH1750_SendCmd(BH1750_POWER_ON);   
    
    // 设置高分辨率连续测量模式
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
    
    // 发送读地址
    // BH1750_ADDR_READ = 0x47（已经包含读位）
    IICSendByte(&bh1750_bus, BH1750_ADDR_READ);
    
    // 检查ACK
    if (IICWaitAck(&bh1750_bus) != SUCCESS) {
        IICStop(&bh1750_bus);
        return 0xFFFF;  // 读取失败
    }
    
    // 读取高字节
    buf[0] = IICReceiveByte(&bh1750_bus);
    IICSendAck(&bh1750_bus);                // 发送ACK
    
    // 读取低字节
    buf[1] = IICReceiveByte(&bh1750_bus);
    IICSendNotAck(&bh1750_bus);             // 发送NACK（最后一个字节）
    
    IICStop(&bh1750_bus);
    
    return ((uint16_t)buf[0] << 8) | buf[1];
}

float BH1750_GetLux(void)
{
    uint16_t raw = BH1750_ReadRaw();
    
    if (raw == 0xFFFF) {
        return -1.0f;  // 读取失败
    }
    
    // 根据模式计算照度
    // H分辨率模式：raw / 1.2
    // H2分辨率模式：raw / 0.5
    // L分辨率模式：raw / 4.0
    
    // 注意：这里假设使用的是H1模式
    // 如果需要支持多种模式，可以添加模式参数
    return (float)raw / 1.2f;
}

void BH1750_Reset(void)
{
    BH1750_SendCmd(BH1750_RESET);
    delay_ms(10);
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