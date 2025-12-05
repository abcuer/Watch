#include "sht31.h"
#include "bsp_iic.h"
#include "stdint.h"
#include "bsp_delay.h"

iic_bus_t sht31_bus = {
    .SDA_PORT = GPIOB,
    .SDA_PIN  = GPIO_PIN_11,
    .SCL_PORT = GPIOB,
    .SCL_PIN  = GPIO_PIN_10,
};

void SHTInit(iic_bus_t *bus)
{
    IIC_Init(bus);
}

static const uint8_t SHT31_CMD_MEASURE[2] = {0x24, 0x00};
static uint8_t SHT31_CalcCRC(uint8_t *data)
{
    uint8_t crc = 0xFF;
    for (uint8_t i = 0; i < 2; i++)
    {
        crc ^= data[i];
        for (uint8_t j = 0; j < 8; j++)
        {
            if (crc & 0x80)
                crc = (crc << 1) ^ 0x31;
            else
                crc = (crc << 1);
        }
    }
    return crc;
}

HAL_StatusTypeDef SHT31_ReadTempHum(iic_bus_t *bus, float *temperature, float *humidity)
{
    uint8_t data[6];

    /* 发送测量命令 */
    IIC_Start(bus);
    if (IIC_SendByte(bus, SHT31_ADDR | 0) != 0) { IIC_Stop(bus); return HAL_ERROR; }
    IIC_SendByte(bus, SHT31_CMD_MEASURE[0]);
    IIC_SendByte(bus, SHT31_CMD_MEASURE[1]);
    IIC_Stop(bus);

    delay_ms(20);     // 典型测量时间 15ms

    /* 读数据 */
    IIC_Start(bus);
    if (IIC_SendByte(bus, SHT31_ADDR | 1) != 0) {
        IIC_Stop(bus);
        return HAL_ERROR;
    }

    for (int i = 0; i < 6; i++)
        data[i] = IIC_ReceiveByte(bus, (i < 5));

    IIC_Stop(bus);

    /* 校验 CRC */
    if (SHT31_CalcCRC(&data[0]) != data[2]) return HAL_ERROR;
    if (SHT31_CalcCRC(&data[3]) != data[5]) return HAL_ERROR;

    /* 解析温湿度 */
    uint16_t rawT = (data[0] << 8) | data[1];
    uint16_t rawH = (data[3] << 8) | data[4];

    *temperature = -45.0f + 175.0f * ((float)rawT / 65535.0f);
    *humidity    = 100.0f * ((float)rawH / 65535.0f);

    return HAL_OK;
}
