#include "MPU6050_I2C.h"

// SDA 输出
void MPU6050_IIC_SDA_OUT(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = MPU6050_IIC_SDA_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;      // 推挽输出
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(MPU6050_IIC_GPIO_PORT, &GPIO_InitStruct);
}

// SDA 输入（带上拉）
void MPU6050_IIC_SDA_IN(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = MPU6050_IIC_SDA_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(MPU6050_IIC_GPIO_PORT, &GPIO_InitStruct);
}

// 整体 IO 初始化
void MPU6050_IIC_IO_Init(void)
{
    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = MPU6050_IIC_SCL_PIN | MPU6050_IIC_SDA_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(MPU6050_IIC_GPIO_PORT, &GPIO_InitStruct);

    MPU6050_IIC_SCL_HIGH();
    MPU6050_IIC_SDA_HIGH();
}

// 发送 START
void MPU6050_IIC_Start(void)
{
    MPU6050_IIC_SDA_OUT();
    MPU6050_IIC_SDA_HIGH();
    MPU6050_IIC_SCL_HIGH();
    MPU6050_IIC_delay_4us();

    MPU6050_IIC_SDA_LOW();   // START
    MPU6050_IIC_delay_4us();
    MPU6050_IIC_SCL_LOW();
}

// STOP
void MPU6050_IIC_Stop(void)
{
    MPU6050_IIC_SDA_OUT();
    MPU6050_IIC_SCL_LOW();
    MPU6050_IIC_SDA_LOW();
    MPU6050_IIC_delay_4us();

    MPU6050_IIC_SCL_HIGH();
    MPU6050_IIC_delay_4us();
    MPU6050_IIC_SDA_HIGH();
    MPU6050_IIC_delay_4us();
}

// 等待 ACK
uint8_t MPU6050_IIC_Wait_Ack(void)
{
    uint8_t ucErrTime = 0;

    MPU6050_IIC_SDA_IN();  // 输入
    MPU6050_IIC_SDA_HIGH();
    MPU6050_IIC_delay_4us();

    MPU6050_IIC_SCL_HIGH();
    MPU6050_IIC_delay_4us();

    while (MPU6050_IIC_SDA_READ())
    {
        ucErrTime++;
        if (ucErrTime > 250)
        {
            MPU6050_IIC_Stop();
            return 1;
        }
    }

    MPU6050_IIC_SCL_LOW();
    return 0;
}

// 发送 ACK / NACK
void MPU6050_IIC_Send_Ack(uint8_t ack)
{
    MPU6050_IIC_SDA_OUT();
    MPU6050_IIC_SCL_LOW();

    if (ack)
        MPU6050_IIC_SDA_HIGH();
    else
        MPU6050_IIC_SDA_LOW();

    MPU6050_IIC_delay_4us();
    MPU6050_IIC_SCL_HIGH();
    MPU6050_IIC_delay_4us();
    MPU6050_IIC_SCL_LOW();
}

// 发送一个字节
void MPU6050_IIC_Send_Byte(uint8_t data)
{
    MPU6050_IIC_SDA_OUT();
    MPU6050_IIC_SCL_LOW();

    for (uint8_t i = 0; i < 8; i++)
    {
        if (data & 0x80)
            MPU6050_IIC_SDA_HIGH();
        else
            MPU6050_IIC_SDA_LOW();

        data <<= 1;

        MPU6050_IIC_delay_4us();
        MPU6050_IIC_SCL_HIGH();
        MPU6050_IIC_delay_4us();
        MPU6050_IIC_SCL_LOW();
    }

    MPU6050_IIC_Wait_Ack();
}

// 读 1 字节
uint8_t MPU6050_IIC_Read_Byte(uint8_t ack)
{
    uint8_t receive = 0;

    MPU6050_IIC_SDA_IN();

    for (uint8_t i = 0; i < 8; i++)
    {
        MPU6050_IIC_SCL_LOW();
        MPU6050_IIC_delay_4us();

        MPU6050_IIC_SCL_HIGH();
        receive <<= 1;

        if (MPU6050_IIC_SDA_READ())
            receive++;

        MPU6050_IIC_delay_4us();
    }

    MPU6050_IIC_SCL_LOW();
    MPU6050_IIC_Send_Ack(ack);

    return receive;
}
