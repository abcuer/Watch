#include "draw.h"
static uint16_t last_x = 0;
static uint16_t last_y = 0;
static uint8_t is_touching = 0;
static uint8_t line_thickness = 3; 

void CST816_Test()
{
	uint8_t finger_num = CST816_Get_FingerNum();
	if (finger_num > 0 && finger_num != 0xFF)
	{
		// 读取坐标数据
		CST816_Get_XY_AXIS();
	}
	else
	{
		CST816_Instance.X_Pos = 0;
		CST816_Instance.Y_Pos = 0;
	}
}

void Touch_DrawLine(void)
{
    uint8_t finger_num = CST816_Get_FingerNum();
    if (finger_num > 0 && finger_num != 0xFF)
    {
        CST816_Get_XY_AXIS();
        uint16_t current_x = CST816_Instance.X_Pos;
        uint16_t current_y = CST816_Instance.Y_Pos;
        if (current_x >= Screen_WIDTH) current_x = Screen_WIDTH - 1;
        if (current_y >= Screen_HEIGHT) current_y = Screen_HEIGHT - 1;
        if (!is_touching)
        {
            if (line_thickness <= 2) {
                Screen_DrawPixel_4px(current_x, current_y, RED);
            } else {
                int8_t radius = line_thickness / 2;
                for (int8_t i = -radius; i <= radius; i++) {
                    for (int8_t j = -radius; j <= radius; j++) {
                        if (i*i + j*j <= radius*radius) {
                            Screen_DrawPixel(current_x + i, current_y + j, RED);
                        }
                    }
                }
            }
            is_touching = 1;
        }
        else
        {
            Screen_DrawThickLine(last_x, last_y, current_x, current_y, BLUE, line_thickness);
        }
        last_x = current_x;
        last_y = current_y;
    }
    else
    {
		last_x = 0;
		last_y = 0;
        is_touching = 0;
    }
}

// 设置线宽的函数
void SetLineThickness(uint8_t thickness)
{
    if (thickness < 1) thickness = 1;
    if (thickness > 10) thickness = 10; 
    line_thickness = thickness;
}