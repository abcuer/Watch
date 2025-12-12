#include "BMP280.h"
#include "bsp_iic.h"
#include <stdint.h>
#include <math.h>

// 定义I2C总线结构体
iic_bus_t bmp280_bus = {
    .IIC_SDA_PORT = GPIOB,
    .IIC_SDA_PIN = GPIO_PIN_11,
    .IIC_SCL_PORT = GPIOB,
    .IIC_SCL_PIN = GPIO_PIN_10
};

BMP280 bmp280_inst;
BMP280* bmp280 = &bmp280_inst;

// 适配函数：确保地址格式正确
static void BMP280_Write_Byte(uint8_t reg, uint8_t data)
{
    // 注意：bsp_iic库函数内部可能已经处理了地址左移
    // 如果读取不到数据，请调整这里的地址格式
    IIC_Write_One_Byte(&bmp280_bus, BMP280_ADDRESS, reg, data);
}

static uint8_t BMP280_Read_Byte(uint8_t reg)
{
    return IIC_Read_One_Byte(&bmp280_bus, BMP280_ADDRESS, reg);
}

// 添加：读取多个连续字节函数（用于调试）
static void BMP280_Read_Multi_Byte(uint8_t reg, uint8_t *data, uint8_t len)
{
    IIC_Read_Multi_Byte(&bmp280_bus, BMP280_ADDRESS, reg, len, data);
}

void Bmp_Init(void)
{
    // 初始化I2C总线
    IICInit(&bmp280_bus);
    
    // 等待传感器上电稳定
    volatile int i;
    for(i = 0; i < 10000; i++); // 简短延时
    
    uint8_t Lsb, Msb;
    
    /********************读取校准参数*********************/
    // 温度传感器的校准值
    Lsb = BMP280_Read_Byte(BMP280_DIG_T1_LSB_REG);
    Msb = BMP280_Read_Byte(BMP280_DIG_T1_MSB_REG);
    bmp280->T1 = (((uint16_t)Msb) << 8) + Lsb;
    
    Lsb = BMP280_Read_Byte(BMP280_DIG_T2_LSB_REG);
    Msb = BMP280_Read_Byte(BMP280_DIG_T2_MSB_REG);
    bmp280->T2 = (((int16_t)Msb) << 8) + Lsb;
    
    Lsb = BMP280_Read_Byte(BMP280_DIG_T3_LSB_REG);
    Msb = BMP280_Read_Byte(BMP280_DIG_T3_MSB_REG);
    bmp280->T3 = (((int16_t)Msb) << 8) + Lsb;
    
    // 气压传感器的校准值
    Lsb = BMP280_Read_Byte(BMP280_DIG_P1_LSB_REG);
    Msb = BMP280_Read_Byte(BMP280_DIG_P1_MSB_REG);
    bmp280->P1 = (((uint16_t)Msb) << 8) + Lsb;
    
    Lsb = BMP280_Read_Byte(BMP280_DIG_P2_LSB_REG);
    Msb = BMP280_Read_Byte(BMP280_DIG_P2_MSB_REG);
    bmp280->P2 = (((int16_t)Msb) << 8) + Lsb;
    
    Lsb = BMP280_Read_Byte(BMP280_DIG_P3_LSB_REG);
    Msb = BMP280_Read_Byte(BMP280_DIG_P3_MSB_REG);
    bmp280->P3 = (((int16_t)Msb) << 8) + Lsb;
    
    Lsb = BMP280_Read_Byte(BMP280_DIG_P4_LSB_REG);
    Msb = BMP280_Read_Byte(BMP280_DIG_P4_MSB_REG);
    bmp280->P4 = (((int16_t)Msb) << 8) + Lsb;
    
    Lsb = BMP280_Read_Byte(BMP280_DIG_P5_LSB_REG);
    Msb = BMP280_Read_Byte(BMP280_DIG_P5_MSB_REG);
    bmp280->P5 = (((int16_t)Msb) << 8) + Lsb;
    
    Lsb = BMP280_Read_Byte(BMP280_DIG_P6_LSB_REG);
    Msb = BMP280_Read_Byte(BMP280_DIG_P6_MSB_REG);
    bmp280->P6 = (((int16_t)Msb) << 8) + Lsb;
    
    Lsb = BMP280_Read_Byte(BMP280_DIG_P7_LSB_REG);
    Msb = BMP280_Read_Byte(BMP280_DIG_P7_MSB_REG);
    bmp280->P7 = (((int16_t)Msb) << 8) + Lsb;
    
    Lsb = BMP280_Read_Byte(BMP280_DIG_P8_LSB_REG);
    Msb = BMP280_Read_Byte(BMP280_DIG_P8_MSB_REG);
    bmp280->P8 = (((int16_t)Msb) << 8) + Lsb;
    
    Lsb = BMP280_Read_Byte(BMP280_DIG_P9_LSB_REG);
    Msb = BMP280_Read_Byte(BMP280_DIG_P9_MSB_REG);
    bmp280->P9 = (((int16_t)Msb) << 8) + Lsb;
    
    // 软复位
    BMP280_Write_Byte(BMP280_RESET_REG, BMP280_RESET_VALUE);
    
    // 短暂延时等待复位完成
    for(i = 0; i < 5000; i++);
    
    // 配置采样模式和滤波器（与原代码完全一致）
    BMP_OVERSAMPLE_MODE BMP_OVERSAMPLE_MODEStructure;
    BMP_OVERSAMPLE_MODEStructure.P_Osample = BMP280_P_MODE_3;
    BMP_OVERSAMPLE_MODEStructure.T_Osample = BMP280_T_MODE_1;
    BMP_OVERSAMPLE_MODEStructure.WORKMODE  = BMP280_NORMAL_MODE;
    BMP280_Set_TemOversamp(&BMP_OVERSAMPLE_MODEStructure);
    
    BMP_CONFIG BMP_CONFIGStructure;
    BMP_CONFIGStructure.T_SB = BMP280_T_SB1;
    BMP_CONFIGStructure.FILTER_COEFFICIENT = BMP280_FILTER_MODE_4;
    BMP_CONFIGStructure.SPI_EN = DISABLE;
    
    BMP280_Set_Standby_FILTER(&BMP_CONFIGStructure);
    
    // 等待传感器稳定
    delay_ms(10);
}

// 以下函数保持原样
void BMP280_Set_TemOversamp(BMP_OVERSAMPLE_MODE *Oversample_Mode)
{
    uint8_t Regtmp;
    Regtmp = ((Oversample_Mode->T_Osample) << 5) |
             ((Oversample_Mode->P_Osample) << 2) |
             (Oversample_Mode->WORKMODE);
    
    BMP280_Write_Byte(BMP280_CTRLMEAS_REG, Regtmp);
}

void BMP280_Set_Standby_FILTER(BMP_CONFIG *BMP_Config)
{
    uint8_t Regtmp;
    Regtmp = ((BMP_Config->T_SB) << 5) |
             ((BMP_Config->FILTER_COEFFICIENT) << 2) |
             (BMP_Config->SPI_EN);
    
    BMP280_Write_Byte(BMP280_CONFIG_REG, Regtmp);
}

uint8_t BMP280_GetStatus(uint8_t status_flag)
{
    uint8_t flag;
    flag = BMP280_Read_Byte(BMP280_STATUS_REG);
    if (flag & status_flag) return SET;
    else return RESET;
}

uint8_t BMP280_ReadID(void)
{
    return BMP280_Read_Byte(BMP280_CHIPID_REG);
}

BMP280_S32_t t_fine;

// 添加：获取原始ADC值的函数（用于调试）
BMP280_S32_t BMP280_Get_Temperature_ADC(void)
{
    uint8_t XLsb, Lsb, Msb;
    XLsb = BMP280_Read_Byte(BMP280_TEMPERATURE_XLSB_REG);
    Lsb  = BMP280_Read_Byte(BMP280_TEMPERATURE_LSB_REG);
    Msb  = BMP280_Read_Byte(BMP280_TEMPERATURE_MSB_REG);
    return ((long)(Msb << 12)) | ((long)(Lsb << 4)) | (XLsb >> 4);
}

BMP280_S32_t BMP280_Get_Pressure_ADC(void)
{
    uint8_t XLsb, Lsb, Msb;
    XLsb = BMP280_Read_Byte(BMP280_PRESSURE_XLSB_REG);
    Lsb  = BMP280_Read_Byte(BMP280_PRESSURE_LSB_REG);
    Msb  = BMP280_Read_Byte(BMP280_PRESSURE_MSB_REG);
    return ((long)(Msb << 12)) | ((long)(Lsb << 4)) | (XLsb >> 4);
}

// 原始补偿算法（保持完全一致）
double bmp280_compensate_T_double(BMP280_S32_t adc_T)
{
    double var1, var2, T;
    var1 = (((double)adc_T) / 16384.0 - ((double)dig_T1) / 1024.0) * ((double)dig_T2);
    var2 = ((((double)adc_T) / 131072.0 - ((double)dig_T1) / 8192.0) *
            (((double)adc_T) / 131072.0 - ((double)dig_T1) / 8192.0)) * ((double)dig_T3);
    t_fine = (BMP280_S32_t)(var1 + var2);
    T = (var1 + var2) / 5120.0;
    return T;
}

double bmp280_compensate_P_double(BMP280_S32_t adc_P)
{
    double var1, var2, p;
    var1 = ((double)t_fine / 2.0) - 64000.0;
    var2 = var1 * var1 * ((double)dig_P6) / 32768.0;
    var2 = var2 + var1 * ((double)dig_P5) * 2.0;
    var2 = (var2 / 4.0) + (((double)dig_P4) * 65536.0);
    var1 = (((double)dig_P3) * var1 * var1 / 524288.0 + ((double)dig_P2) * var1) / 524288.0;
    var1 = (1.0 + var1 / 32768.0) * ((double)dig_P1);
    if (var1 == 0.0)
    {
        return 0;
    }
    p = 1048576.0 - (double)adc_P;
    p = (p - (var2 / 4096.0)) * 6250.0 / var1;
    var1 = ((double)dig_P9) * p * p / 2147483648.0;
    var2 = p * ((double)dig_P8) / 32768.0;
    p = p + (var1 + var2 + ((double)dig_P7)) / 16.0;
    return p;
}

double BMP280_Get_Pressure(void)
{
    BMP280_S32_t adc_P = BMP280_Get_Pressure_ADC();
    return bmp280_compensate_P_double(adc_P);
}

double PressureToAltitude(double pressure, double seaLevelPressure)
{
    return 44330.0 * (1.0 - pow(pressure / seaLevelPressure, 0.19029495718363465));
}

double PressureToAltitudeStd(double pressure)
{
    return PressureToAltitude(pressure, 101325.0);
}

double BMP280_Get_Temperature(void)
{
    BMP280_S32_t adc_T = BMP280_Get_Temperature_ADC();
    return bmp280_compensate_T_double(adc_T);
}

// 添加：快速初始化函数（简化版，用于测试）
void Bmp_Quick_Init(void)
{
    IICInit(&bmp280_bus);
    
    // 简单配置：立即进入正常模式
    BMP280_Write_Byte(BMP280_CTRLMEAS_REG, 0x27); // 温度x1，气压x1，正常模式
    
    delay_ms(10);
}