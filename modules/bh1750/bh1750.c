#include "bh1750.h"
#include "bsp_delay.h"
#include "gpio.h"

/********************************************
 * GPIO 操作宏
 ********************************************/
#define IIC_SCL_H   HAL_GPIO_WritePin(BH1750_GPIO_PORT, BH1750_SCL_GPIO_PIN, GPIO_PIN_SET)
#define IIC_SCL_L   HAL_GPIO_WritePin(BH1750_GPIO_PORT, BH1750_SCL_GPIO_PIN, GPIO_PIN_RESET)

#define IIC_SDA_H   HAL_GPIO_WritePin(BH1750_GPIO_PORT, BH1750_SDA_GPIO_PIN, GPIO_PIN_SET)
#define IIC_SDA_L   HAL_GPIO_WritePin(BH1750_GPIO_PORT, BH1750_SDA_GPIO_PIN, GPIO_PIN_RESET)

#define READ_SDA    HAL_GPIO_ReadPin(BH1750_GPIO_PORT, BH1750_SDA_GPIO_PIN)

/********************************************
 * SDA 设置为输出（开漏）
 ********************************************/
void SDA_OUT(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin   = BH1750_SDA_GPIO_PIN;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull  = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(BH1750_GPIO_PORT, &GPIO_InitStruct);
}

/********************************************
 * SDA 设置为输入
 ********************************************/
void SDA_IN(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin   = BH1750_SDA_GPIO_PIN;
    GPIO_InitStruct.Mode  = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull  = GPIO_PULLUP;
    HAL_GPIO_Init(BH1750_GPIO_PORT, &GPIO_InitStruct);
}

/********************************************
 * 初始化 BH1750 GPIO + 设备
 ********************************************/
void BH1750_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    BH1750_GPIO_CLK_ENABLE();

    /* SCL + SDA：开漏输出，带上拉 */
    GPIO_InitStruct.Pin = BH1750_SCL_GPIO_PIN | BH1750_SDA_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(BH1750_GPIO_PORT, &GPIO_InitStruct);

    IIC_SCL_H;
    IIC_SDA_H;

    /* 设备初始化 */
    bh_data_send(BHPowOn);
    bh_data_send(BHReset);
    bh_data_send(BHModeH1);   // 默认高分辨率120ms

    HAL_Delay(180);           // 等待首次测量完成
}

/********************************************
 * I2C Start
 ********************************************/
void BH1750_IIC_Start(void)
{
    SDA_OUT();
    IIC_SDA_H;
    IIC_SCL_H;
    delay_us(5);

    IIC_SDA_L;
    delay_us(5);
    IIC_SCL_L;
}

/********************************************
 * I2C Stop
 ********************************************/
void BH1750_IIC_Stop(void)
{
    SDA_OUT();
    IIC_SCL_L;
    IIC_SDA_L;
    delay_us(5);

    IIC_SCL_H;
    IIC_SDA_H;
    delay_us(5);
}

/********************************************
 * 等待 ACK
 ********************************************/
uint8_t BH1750_IIC_Wait_Ack(void)
{
    uint16_t ucErr = 0;

    SDA_IN();
    IIC_SDA_H;
    delay_us(2);
    IIC_SCL_H;
    delay_us(2);

    while (READ_SDA)
    {
        ucErr++;
        if (ucErr > 300)
        {
            BH1750_IIC_Stop();
            return 1;  // 无响应
        }
    }

    IIC_SCL_L;
    return 0;
}

/********************************************
 * 发送 ACK
 ********************************************/
void BH1750_IIC_Ack(void)
{
    IIC_SCL_L;
    SDA_OUT();
    IIC_SDA_L;
    delay_us(2);

    IIC_SCL_H;
    delay_us(2);
    IIC_SCL_L;
}

/********************************************
 * 发送 NACK
 ********************************************/
void BH1750_IIC_NAck(void)
{
    IIC_SCL_L;
    SDA_OUT();
    IIC_SDA_H;
    delay_us(2);

    IIC_SCL_H;
    delay_us(2);
    IIC_SCL_L;
}

/********************************************
 * 发送一个字节
 ********************************************/
void BH1750_IIC_Send_Byte(uint8_t txd)
{
    uint8_t t;
    SDA_OUT();
    IIC_SCL_L;

    for (t = 0; t < 8; t++)
    {
        if (txd & 0x80) IIC_SDA_H;
        else            IIC_SDA_L;

        txd <<= 1;
        delay_us(2);
        IIC_SCL_H;
        delay_us(2);
        IIC_SCL_L;
    }
}

/********************************************
 * 读取一个字节
 ********************************************/
uint8_t BH1750_IIC_Read_Byte(uint8_t ack)
{
    uint8_t i, receive = 0;

    SDA_IN();

    for (i = 0; i < 8; i++)
    {
        IIC_SCL_L;
        delay_us(2);
        IIC_SCL_H;
        receive <<= 1;
        if (READ_SDA) receive++;
        delay_us(2);
    }

    if (ack) BH1750_IIC_Ack();
    else     BH1750_IIC_NAck();

    return receive;
}

/********************************************
 * 发送单字节命令
 ********************************************/
void bh_data_send(uint8_t command)
{
    do {
        BH1750_IIC_Start();
        BH1750_IIC_Send_Byte(BHAddWrite);
    } while (BH1750_IIC_Wait_Ack());

    BH1750_IIC_Send_Byte(command);
    BH1750_IIC_Wait_Ack();

    BH1750_IIC_Stop();
}

/********************************************
 * 读取两字节数据
 ********************************************/
uint16_t bh_data_read(void)
{
    uint16_t buf;

    BH1750_IIC_Start();
    BH1750_IIC_Send_Byte(BHAddRead);
    BH1750_IIC_Wait_Ack();

    buf  = BH1750_IIC_Read_Byte(1);
    buf <<= 8;
    buf |= BH1750_IIC_Read_Byte(0);

    BH1750_IIC_Stop();
    return buf;
}

/********************************************
 * 获取光照强度（lux）
 ********************************************/
float BH1750_GetLux(void)
{
    uint16_t raw = bh_data_read();
    return raw / 1.2f;   // 数据手册：1 lx = raw / 1.2
}
