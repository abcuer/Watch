#include "test.h"

extern CST816_Info CST816_Instance;
void cst816_test()
{
    Screen_Fill_Color(WHITE);
    // 绘制测试界面
    Screen_Fill_Rectangle(0, 0, Screen_WIDTH, 20, BLUE);
    uint8_t finger_num = 0;
    while (1) {
        // 获取触摸状态
        finger_num = CST816_Get_FingerNum();
        if (finger_num > 0 && finger_num != 0xFF) {
            // 读取坐标
            CST816_Get_XY_AXIS();
            
            uint16_t x = CST816_Instance.X_Pos;
            uint16_t y = CST816_Instance.Y_Pos;
            
            // 验证坐标范围
            if (x < Screen_WIDTH && y < Screen_HEIGHT) {
                
                // 绘制触摸点
                Screen_DrawPixel(x, y, RED);
                
                // 绘制一个较大的圆点
                for (int i = -2; i <= 2; i++) {
                    for (int j = -2; j <= 2; j++) {
                        if (x + i >= 0 && x + i < Screen_WIDTH &&
                            y + j >= 0 && y + j < Screen_HEIGHT) {
                            Screen_DrawPixel(x + i, y + j, RED);
                        }
                    }
                }
                
                // 在底部显示坐标
                char buf[32];
                sprintf(buf, "X:%3d Y:%3d F:%d", x, y, finger_num);
                // 清空坐标显示区域
                Screen_Fill_Rectangle(0, Screen_HEIGHT - 20, Screen_WIDTH, 20, BLACK);
            }
        } else {
            // 没有触摸时，延时一下避免CPU占用过高
            HAL_Delay(10);
        }
    }
}


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
MPU_t mpu;

MPU_Init();
MPU_Get_Angle(&mpu);
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
    BloodGetData();
    delay_ms(10);
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