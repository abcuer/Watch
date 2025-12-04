#include "bme280.h"

iic_bus_t bme280_bus = {
    .SDA_PORT = GPIOB,
    .SDA_PIN  = GPIO_PIN_9,
    .SCL_PORT = GPIOB,
    .SCL_PIN  = GPIO_PIN_8,
};

static void BME280_WriteReg(BME280_t *dev, uint8_t reg, uint8_t data)
{
    IIC_Start(dev->bus);
    IIC_WriteByte(dev->bus, (BME280_ADDR << 1) | 0);
    IIC_WaitAck(dev->bus);

    IIC_WriteByte(dev->bus, reg);
    IIC_WaitAck(dev->bus);

    IIC_WriteByte(dev->bus, data);
    IIC_WaitAck(dev->bus);

    IIC_Stop(dev->bus);
}

static void BME280_ReadBytes(BME280_t *dev, uint8_t reg, uint8_t *buf, uint8_t len)
{
    IIC_Start(dev->bus);
    IIC_WriteByte(dev->bus, (BME280_ADDR << 1) | 0);
    IIC_WaitAck(dev->bus);

    IIC_WriteByte(dev->bus, reg);
    IIC_WaitAck(dev->bus);

    IIC_Start(dev->bus);
    IIC_WriteByte(dev->bus, (BME280_ADDR << 1) | 1);
    IIC_WaitAck(dev->bus);

    for(uint8_t i=0; i<len; i++)
    {
        buf[i] = IIC_WriteByte(dev->bus, (i != (len-1)));
    }
    IIC_Stop(dev->bus);
}

static void BME280_ReadTrim(BME280_t *dev)
{
    uint8_t d[32];
    uint8_t i=0;

    BME280_ReadBytes(dev, 0x88, d, 24);
    i = 24;

    BME280_ReadBytes(dev, 0xA1, &d[i], 1); i++;

    BME280_ReadBytes(dev, 0xE1, &d[i], 7);

    dev->dig_T1 = (d[1] << 8) | d[0];
    dev->dig_T2 = (d[3] << 8) | d[2];
    dev->dig_T3 = (d[5] << 8) | d[4];

    dev->dig_P1 = (d[7] << 8) | d[6];
    dev->dig_P2 = (d[9] << 8) | d[8];
    dev->dig_P3 = (d[11] << 8) | d[10];
    dev->dig_P4 = (d[13] << 8) | d[12];
    dev->dig_P5 = (d[15] << 8) | d[14];
    dev->dig_P6 = (d[17] << 8) | d[16];
    dev->dig_P7 = (d[19] << 8) | d[18];
    dev->dig_P8 = (d[21] << 8) | d[20];
    dev->dig_P9 = (d[23] << 8) | d[22];

    dev->dig_H1 = d[24];
    dev->dig_H2 = (d[26] << 8) | d[25];
    dev->dig_H3 = d[27];
    dev->dig_H4 = (d[28] << 4) | (d[29] & 0x0F);
    dev->dig_H5 = (d[30] << 4) | (d[29] >> 4);
    dev->dig_H6 = d[31];
}

void BME280_Init(BME280_t *dev, iic_bus_t *bus)
{
    dev->bus = bus;

    // 湿度 oversampling ×1
    BME280_WriteReg(dev, 0xF2, 0x01);

    // 温度、压力 oversampling ×1，Normal mode
    BME280_WriteReg(dev, 0xF4, 0x27);

    // Standby 1000ms, filter off
    BME280_WriteReg(dev, 0xF5, 0xA0);

    BME280_ReadTrim(dev);
}

static int32_t BME280_CalcT(BME280_t *dev, int32_t adc_T)
{
    int32_t var1, var2, T;

    var1 = ((((adc_T >> 3) - ((int32_t)dev->dig_T1 << 1))) * ((int32_t)dev->dig_T2)) >> 11;
    var2 = (((((adc_T >> 4) - ((int32_t)dev->dig_T1)) * ((adc_T >> 4) - ((int32_t)dev->dig_T1))) >> 12) *
            ((int32_t)dev->dig_T3)) >> 14;

    dev->t_fine = var1 + var2;
    T = (dev->t_fine * 5 + 128) >> 8;
    return T;
}

static uint32_t BME280_CalcP(BME280_t *dev, int32_t adc_P)
{
    int64_t var1, var2, p;

    var1 = ((int64_t)dev->t_fine) - 128000;
    var2 = var1 * var1 * (int64_t)dev->dig_P6;
    var2 = var2 + ((var1 * (int64_t)dev->dig_P5) << 17);
    var2 = var2 + (((int64_t)dev->dig_P4) << 35);
    var1 = ((var1 * var1 * (int64_t)dev->dig_P3) >> 8) + ((var1 * (int64_t)dev->dig_P2) << 12);
    var1 = (((((int64_t)1 << 47) + var1)) * ((int64_t)dev->dig_P1)) >> 33;

    if (var1 == 0) return 0;

    p = 1048576 - adc_P;
    p = (((p << 31) - var2) * 3125) / var1;

    var1 = (((int64_t)dev->dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    var2 = (((int64_t)dev->dig_P8) * p) >> 19;

    p = ((p + var1 + var2) >> 8) + (((int64_t)dev->dig_P7) << 4);
    return (uint32_t)p;
}

static uint32_t BME280_CalcH(BME280_t *dev, int32_t adc_H)
{
    int32_t v_x1;

    v_x1 = dev->t_fine - 76800;

    v_x1 = (((((adc_H << 14) - ((int32_t)dev->dig_H4 << 20) -
               ((int32_t)dev->dig_H5 * v_x1)) + 16384) >> 15) *
            (((((((v_x1 * (int32_t)dev->dig_H6) >> 10) *
                 (((v_x1 * (int32_t)dev->dig_H3) >> 11) + 32768)) >> 10) + 2097152) *
              (int32_t)dev->dig_H2 + 8192) >> 14));

    v_x1 -= (((v_x1 >> 15) * (v_x1 >> 15)) >> 7) * (int32_t)dev->dig_H1 >> 4;

    if(v_x1 < 0) v_x1 = 0;
    if(v_x1 > 419430400) v_x1 = 419430400;

    return (uint32_t)(v_x1 >> 12);
}

void BME280_ReadData(BME280_t *dev, float *T, float *P, float *H)
{
    uint8_t d[8];
    int32_t adc_T, adc_P, adc_H;

    BME280_ReadBytes(dev, 0xF7, d, 8);

    adc_P = (d[0] << 12) | (d[1] << 4) | (d[2] >> 4);
    adc_T = (d[3] << 12) | (d[4] << 4) | (d[5] >> 4);
    adc_H = (d[6] << 8)  | d[7];

    int32_t T100 = BME280_CalcT(dev, adc_T);
    uint32_t P100 = BME280_CalcP(dev, adc_P);
    uint32_t H1024 = BME280_CalcH(dev, adc_H);

    *T = T100 / 100.0f;
    *P = P100 / 25600.0f;   // 转换为 hPa
    *H = H1024 / 1024.0f;
}
