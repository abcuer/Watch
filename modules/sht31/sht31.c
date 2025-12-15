#include "sht31.h"
#include "bsp_iic.h"
#include "stdint.h"
#include "bsp_delay.h"

iic_bus_t sht31_bus = {
    .IIC_SDA_PORT = GPIOB,
    .IIC_SDA_PIN = GPIO_PIN_11,
    .IIC_SCL_PORT = GPIOB,
    .IIC_SCL_PIN = GPIO_PIN_10
};

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

HAL_StatusTypeDef SHT31_ReadTempHum(float *temperature, float *humidity)
{
    uint8_t data[6];

    IICStart(&sht31_bus);
    IICSendByte(&sht31_bus, SHT31_ADDR << 1);
    if (IICWaitAck(&sht31_bus) != SUCCESS) {
        IICStop(&sht31_bus);
        return HAL_ERROR;
    }
    IICSendByte(&sht31_bus, SHT31_CMD_MEASURE[0]);
    IICWaitAck(&sht31_bus);
    IICSendByte(&sht31_bus, SHT31_CMD_MEASURE[1]);
    IICWaitAck(&sht31_bus);
    IICStop(&sht31_bus);

    delay_ms(15);     // 典型测量时间 15ms

    IICStart(&sht31_bus);
    IICSendByte(&sht31_bus, (SHT31_ADDR << 1) | 0x01);
    if (IICWaitAck(&sht31_bus) != SUCCESS) {
        IICStop(&sht31_bus);
        return HAL_ERROR;
    }
    
    for (int i = 0; i < 6; i++)
    {
        data[i] = IICReceiveByte(&sht31_bus);
        if (i < 5)
            IICSendAck(&sht31_bus);      // 前5个字节发ACK
        else
            IICSendNotAck(&sht31_bus);   // 最后一个字节发NACK
    }
    IICStop(&sht31_bus);

    if (SHT31_CalcCRC(&data[0]) != data[2]) return HAL_ERROR;
    if (SHT31_CalcCRC(&data[3]) != data[5]) return HAL_ERROR;

    uint16_t rawT = (data[0] << 8) | data[1];
    uint16_t rawH = (data[3] << 8) | data[4];

    // 温度计算公式: -45 + 175 * (rawT / 65535)
    *temperature = -45.0f + 175.0f * ((float)rawT / 65535.0f);
    // 湿度计算公式: 100 * (rawH / 65535)
    *humidity = 100.0f * ((float)rawH / 65535.0f);

    return HAL_OK;
}
void SHTInit(void)
{
    IICInit(&sht31_bus);
}