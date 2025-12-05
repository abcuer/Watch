#include "test.h"

/*  MPU6050 使用示例
short ax, ay, az;
short gx, gy, gz;
float temp;

MPU_Init();

MPU_Get_Accelerometer(&ax, &ay, &az);
MPU_Get_Gyroscope(&gx, &gy, &gz);
temp = MPU_Get_Temperature();
delay_ms(10);
*/

/*  BH1750 使用示例
float lux;

BH1750_Init();

lux = BH1750_GetLux();
delay_ms(500);
*/

/*  max30102 使用示例
uint16_t SPO2data;  //用于存储最终要显示在血氧检测功能的一级菜单中的数据
uint16_t Heartdata; //用于存储最终要显示在心率检测功能的一级菜单中的数据

MAX30102_Init();
blood_Loop(SPO2data, Heartdata);
delay_ms(500);
*/

/*  BMP280 使用示例
uint8_t ID;
float temp, pre, alt;

Bmp_Init();
ID=BMP280_ReadID();          //获得ID号
temp = BMP280_Get_Temperature();
pre = BMP280_Get_Pressure();
alt = PressureToAltitude(pre);
delay_ms(500);
*/

/*  SHT31 使用示例
float temp, hum;

SHTInit(&sht31_bus);
void test()
{
    SHT31_ReadTempHum(&sht31_bus, &temp, &hum)
    delay_ms(100);
}
*/

/*  屏幕  使用示例
Screen_Init();

void test()
{
    Screen_Test();
}
*/