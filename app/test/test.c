#include "test.h"

/* 触摸屏使用示例
  uint8_t check;
  uint8_t check_data[4];
  uint16_t x, y;

Screen_Init();

check = ChechkPush();
if(check != 0)
{
    ReadPushDate(check_data);
    uint8_t xh = (check_data[1] >> 4) & 0x0F;
    uint8_t yh = (check_data[1]     ) & 0x0F;

    uint8_t xl = check_data[2];
    uint8_t yl = check_data[3];

    x = (xh << 8) | xl;
    y = (yh << 8) | yl;

}
check = 0;
*/


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
bmp280_t bmp;

Bmp_Init();

bmp.id = BMP280_ReadID();
bmp.temp = BMP280_Get_Temperature();
bmp.pre = BMP280_Get_Pressure();
bmp.alt = PressureToAltitude(bmp.pre, 101325.0);
delay_ms(100);
*/

/*  SHT31 使用示例
SHT31_t sht31;
SHTInit();

SHT31_ReadTempHum(&sht31.temp, &sht31.hum);
delay_ms(100);
*/

/*  屏幕  使用示例
Screen_Init();

void test()
{
    Screen_Test();
}
*/