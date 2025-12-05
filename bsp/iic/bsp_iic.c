#include "bsp_iic.h"
#include "bsp_delay.h"

// SDA 输出
static void SDA_OUT(iic_bus_t *bus)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = bus->SDA_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(bus->SDA_PORT, &GPIO_InitStruct);
}

// SDA 输入
static void SDA_IN(iic_bus_t *bus)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = bus->SDA_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(bus->SDA_PORT, &GPIO_InitStruct);
}

// 初始化
void IIC_Init(iic_bus_t *bus)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

    GPIO_InitStruct.Pin = bus->SCL_PIN;
    HAL_GPIO_Init(bus->SCL_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = bus->SDA_PIN;
    HAL_GPIO_Init(bus->SDA_PORT, &GPIO_InitStruct);

    HAL_GPIO_WritePin(bus->SCL_PORT, bus->SCL_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(bus->SDA_PORT, bus->SDA_PIN, GPIO_PIN_SET);
}
/**************** I2C 基础操作 ****************/
void IIC_Start(iic_bus_t *bus)
{
    SDA_OUT(bus);
    HAL_GPIO_WritePin(bus->SDA_PORT, bus->SDA_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(bus->SCL_PORT, bus->SCL_PIN, GPIO_PIN_SET);
    delay_us(5);

    HAL_GPIO_WritePin(bus->SDA_PORT, bus->SDA_PIN, GPIO_PIN_RESET);
    delay_us(5);
    HAL_GPIO_WritePin(bus->SCL_PORT, bus->SCL_PIN, GPIO_PIN_RESET);
}

void IIC_Stop(iic_bus_t *bus)
{
    SDA_OUT(bus);
    HAL_GPIO_WritePin(bus->SDA_PORT, bus->SDA_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(bus->SCL_PORT, bus->SCL_PIN, GPIO_PIN_SET);
    delay_us(5);
    HAL_GPIO_WritePin(bus->SDA_PORT, bus->SDA_PIN, GPIO_PIN_SET);
    delay_us(5);
}
/**************** I2C 等待 ACK ****************/
uint8_t IIC_WaitAck(iic_bus_t *bus)
{
    uint8_t ack;

    SDA_IN(bus);                         // SDA 设置为输入
    HAL_GPIO_WritePin(bus->SDA_PORT, bus->SDA_PIN, GPIO_PIN_SET);
    delay_us(1);

    HAL_GPIO_WritePin(bus->SCL_PORT, bus->SCL_PIN, GPIO_PIN_SET);
    delay_us(5);

    ack = HAL_GPIO_ReadPin(bus->SDA_PORT, bus->SDA_PIN);   // 0 = ACK, 1 = NACK

    HAL_GPIO_WritePin(bus->SCL_PORT, bus->SCL_PIN, GPIO_PIN_RESET);
    return ack;
}

/**************** I2C 发送 ACK ****************/
void IIC_SendAck(iic_bus_t *bus)
{
    HAL_GPIO_WritePin(bus->SCL_PORT, bus->SCL_PIN, GPIO_PIN_RESET);

    SDA_OUT(bus);
    HAL_GPIO_WritePin(bus->SDA_PORT, bus->SDA_PIN, GPIO_PIN_RESET);
    delay_us(2);

    HAL_GPIO_WritePin(bus->SCL_PORT, bus->SCL_PIN, GPIO_PIN_SET);
    delay_us(5);

    HAL_GPIO_WritePin(bus->SCL_PORT, bus->SCL_PIN, GPIO_PIN_RESET);
}

/**************** I2C 发送 NACK ****************/
void IIC_SendNotAck(iic_bus_t *bus)
{
    HAL_GPIO_WritePin(bus->SCL_PORT, bus->SCL_PIN, GPIO_PIN_RESET);

    SDA_OUT(bus);
    HAL_GPIO_WritePin(bus->SDA_PORT, bus->SDA_PIN, GPIO_PIN_SET);
    delay_us(2);

    HAL_GPIO_WritePin(bus->SCL_PORT, bus->SCL_PIN, GPIO_PIN_SET);
    delay_us(5);

    HAL_GPIO_WritePin(bus->SCL_PORT, bus->SCL_PIN, GPIO_PIN_RESET);
}

uint8_t IIC_SendByte(iic_bus_t *bus, uint8_t data)
{
    uint8_t ack;

    SDA_OUT(bus);
    for (uint8_t i = 0; i < 8; i++)
    {
        HAL_GPIO_WritePin(bus->SCL_PORT, bus->SCL_PIN, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(bus->SDA_PORT, bus->SDA_PIN, (data & 0x80) ? GPIO_PIN_SET : GPIO_PIN_RESET);
        data <<= 1;

        delay_us(5);
        HAL_GPIO_WritePin(bus->SCL_PORT, bus->SCL_PIN, GPIO_PIN_SET);
        delay_us(5);
    }

    HAL_GPIO_WritePin(bus->SCL_PORT, bus->SCL_PIN, GPIO_PIN_RESET);
    SDA_IN(bus);
    delay_us(5);

    HAL_GPIO_WritePin(bus->SCL_PORT, bus->SCL_PIN, GPIO_PIN_SET);
    // ack = 0 表示从机应答成功
    ack = HAL_GPIO_ReadPin(bus->SDA_PORT, bus->SDA_PIN);
    delay_us(5);

    HAL_GPIO_WritePin(bus->SCL_PORT, bus->SCL_PIN, GPIO_PIN_RESET);
    SDA_OUT(bus);

    return ack;
}

/**************** I2C 接收 1 Byte ****************/
uint8_t IIC_ReceiveByte(iic_bus_t *bus, uint8_t ack)
{
    uint8_t value = 0;

    SDA_IN(bus);
    for (uint8_t i = 0; i < 8; i++)
    {
        HAL_GPIO_WritePin(bus->SCL_PORT, bus->SCL_PIN, GPIO_PIN_RESET);
        delay_us(2);

        HAL_GPIO_WritePin(bus->SCL_PORT, bus->SCL_PIN, GPIO_PIN_SET);
        value <<= 1;
        if (HAL_GPIO_ReadPin(bus->SDA_PORT, bus->SDA_PIN))
            value |= 0x01;

        delay_us(5);
    }

    if (ack)
        IIC_SendAck(bus);
    else
        IIC_SendNotAck(bus);

    return value;
}

uint8_t IIC_Write_One_Byte(iic_bus_t *bus, uint8_t dev_addr, uint8_t reg, uint8_t data)
{
    IIC_Start(bus);
    IIC_SendByte(bus, dev_addr);   // 写地址
    IIC_WaitAck(bus);

    IIC_SendByte(bus, reg);        // 寄存器地址
    IIC_WaitAck(bus);

    IIC_SendByte(bus, data);       // 数据
    IIC_WaitAck(bus);

    IIC_Stop(bus);
    return 1;
}

uint8_t IIC_Write_Multi_Byte(iic_bus_t *bus, uint8_t dev_addr, uint8_t reg, uint8_t *buf, uint8_t len)
{
    IIC_Start(bus);
    IIC_SendByte(bus, dev_addr);   // 写地址
    IIC_WaitAck(bus);

    IIC_SendByte(bus, reg);        // 起始寄存器
    IIC_WaitAck(bus);

    for (uint8_t i = 0; i < len; i++)
    {
        IIC_SendByte(bus, buf[i]); // 顺序写入
        if (IIC_WaitAck(bus)) return 0; // NACK
    }

    IIC_Stop(bus);
    return 1;
}


uint8_t IIC_Read_One_Byte(iic_bus_t *bus, uint8_t dev_addr, uint8_t reg)
{
    uint8_t data;

    IIC_Start(bus);
    IIC_SendByte(bus, dev_addr);   // 写地址
    IIC_WaitAck(bus);

    IIC_SendByte(bus, reg);        // 寄存器地址
    IIC_WaitAck(bus);

    IIC_Start(bus);
    IIC_SendByte(bus, dev_addr | 0x01);   // 读地址
    IIC_WaitAck(bus);

    data = IIC_ReceiveByte(bus, 0);  // 最后一个字节 -> NACK

    IIC_Stop(bus);
    return data;
}

void IIC_Read_Multi_Byte(iic_bus_t *bus, uint8_t dev_addr, uint8_t reg, uint8_t *buf, uint8_t len)
{
    IIC_Start(bus);
    IIC_SendByte(bus, dev_addr);    // 写地址
    IIC_WaitAck(bus);

    IIC_SendByte(bus, reg);         // 寄存器地址
    IIC_WaitAck(bus);

    IIC_Start(bus);
    IIC_SendByte(bus, dev_addr | 0x01);   // 读地址
    IIC_WaitAck(bus);

    for (uint8_t i = 0; i < len; i++)
    {
        buf[i] = IIC_ReceiveByte(bus, (i != len - 1));
        // 最后一个字节 NACK
    }

    IIC_Stop(bus);
}
