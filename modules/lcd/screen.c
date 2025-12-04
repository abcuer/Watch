#include "screen.h"

uint16_t DMA_MIN_SIZE = 16;

 #define HOR_LEN 	5	
uint8_t disp_buf[Screen_WIDTH * HOR_LEN];

static void Screen_WriteCommand(uint8_t cmd)
{
	Screen_Select();
	Screen_DC_Clr();
	HAL_SPI_Transmit(&Screen_SPI_PORT, &cmd, sizeof(cmd), HAL_MAX_DELAY);
	Screen_UnSelect();
}


static void Screen_WriteData(uint8_t *buff, size_t buff_size)
{
	Screen_Select();
	Screen_DC_Set();

	while (buff_size > 0) {
		uint16_t chunk_size = buff_size > 65535 ? 65535 : buff_size;
		HAL_SPI_Transmit(&Screen_SPI_PORT, buff, chunk_size, HAL_MAX_DELAY);
		buff += chunk_size;
		buff_size -= chunk_size;
	}

	Screen_UnSelect();
}

static void Screen_WriteSmallData(uint8_t data)
{
	Screen_Select();
	Screen_DC_Set();
	HAL_SPI_Transmit(&Screen_SPI_PORT, &data, sizeof(data), HAL_MAX_DELAY);
	Screen_UnSelect();
}


void Screen_SetRotation(uint8_t m)
{
	Screen_WriteCommand(Screen_MADCTL);	// MADCTL
	switch (m) {
	case 0:
		Screen_WriteSmallData(Screen_MADCTL_MX | Screen_MADCTL_MY | Screen_MADCTL_RGB);
		break;
	case 1:
		Screen_WriteSmallData(Screen_MADCTL_MY | Screen_MADCTL_MV | Screen_MADCTL_RGB);
		break;
	case 2:
		Screen_WriteSmallData(Screen_MADCTL_RGB);
		break;
	case 3:
		Screen_WriteSmallData(Screen_MADCTL_MX | Screen_MADCTL_MV | Screen_MADCTL_RGB);
		break;
	default:
		break;
	}
}


static void Screen_SetAddressWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
	Screen_Select();
	uint16_t x_start = x0 + X_SHIFT, x_end = x1 + X_SHIFT;
	uint16_t y_start = y0 + Y_SHIFT, y_end = y1 + Y_SHIFT;

	Screen_WriteCommand(Screen_CASET); 
	{
		uint8_t data[] = {x_start >> 8, x_start & 0xFF, x_end >> 8, x_end & 0xFF};
		Screen_WriteData(data, sizeof(data));
	}

	Screen_WriteCommand(Screen_RASET);
	{
		uint8_t data[] = {y_start >> 8, y_start & 0xFF, y_end >> 8, y_end & 0xFF};
		Screen_WriteData(data, sizeof(data));
	}
	Screen_WriteCommand(Screen_RAMWR);
	Screen_UnSelect();
}


void Screen_Init(void)
{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, 1);
	HAL_Delay(10);
    Screen_RST_Clr();
    HAL_Delay(10);
    Screen_RST_Set();
    HAL_Delay(20);

    Screen_WriteCommand(Screen_COLMOD);		//	Set color mode
    Screen_WriteSmallData(Screen_COLOR_MODE_16bit);
  	Screen_WriteCommand(0xB2);				//	Porch control
	{
		uint8_t data[] = {0x0C, 0x0C, 0x00, 0x33, 0x33};
		Screen_WriteData(data, sizeof(data));
	}
	Screen_SetRotation(Screen_ROTATION);	//	MADCTL (Display Rotation)
	

    Screen_WriteCommand(0XB7);				
    Screen_WriteSmallData(0x35);			
    Screen_WriteCommand(0xBB);				
    Screen_WriteSmallData(0x19);			
    Screen_WriteCommand(0xC0);				
    Screen_WriteSmallData (0x2C);			
    Screen_WriteCommand (0xC2);				
    Screen_WriteSmallData (0x01);			
    Screen_WriteCommand (0xC3);				
    Screen_WriteSmallData (0x12);			
    Screen_WriteCommand (0xC4);				
    Screen_WriteSmallData (0x20);			
    Screen_WriteCommand (0xC6);				
    Screen_WriteSmallData (0x0F);			
    Screen_WriteCommand (0xD0);				
    Screen_WriteSmallData (0xA4);			
    Screen_WriteSmallData (0xA1);			


	Screen_WriteCommand(0xE0);
	{
		uint8_t data[] = {0xD0, 0x04, 0x0D, 0x11, 0x13, 0x2B, 0x3F, 0x54, 0x4C, 0x18, 0x0D, 0x0B, 0x1F, 0x23};
		Screen_WriteData(data, sizeof(data));
	}

    Screen_WriteCommand(0xE1);
	{
		uint8_t data[] = {0xD0, 0x04, 0x0C, 0x11, 0x13, 0x2C, 0x3F, 0x44, 0x51, 0x2F, 0x1F, 0x1F, 0x20, 0x23};
		Screen_WriteData(data, sizeof(data));
	}
    Screen_WriteCommand (Screen_INVON);	
	Screen_WriteCommand (Screen_SLPOUT);	
  	Screen_WriteCommand (Screen_NORON);	
  	Screen_WriteCommand (Screen_DISPON);

	HAL_Delay(50);
	Screen_Fill_Color(BLACK);				//	Fill with Black.
}


void Screen_Fill_Color(uint16_t color)
{
		uint16_t i;
	Screen_SetAddressWindow(0, 0, Screen_WIDTH - 1, Screen_HEIGHT - 1);
	Screen_Select();
				for (int j = 0; j < sizeof(disp_buf); j++) 
				{
					disp_buf[j] = color;
				}
		uint16_t j;
		for (i = 0; i < Screen_WIDTH; i++)
				for (j = 0; j < Screen_HEIGHT; j++) {
					uint8_t data[] = {color >> 8, color & 0xFF};
					Screen_WriteData(data, sizeof(data));
				}
	Screen_UnSelect();
}

void Screen_Fill_Rectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color) {
   
    Screen_WriteCommand(0x2A);  
    Screen_WriteSmallData(x >> 8);    
    Screen_WriteSmallData(x & 0xFF);  
    Screen_WriteSmallData((x + width - 1) >> 8);   
    Screen_WriteSmallData((x + width - 1) & 0xFF); 

    Screen_WriteCommand(0x2B); 
    Screen_WriteSmallData(y >> 8);   
    Screen_WriteSmallData(y & 0xFF);  
    Screen_WriteSmallData((y + height - 1) >> 8);  
    Screen_WriteSmallData((y + height - 1) & 0xFF);

    Screen_WriteCommand(0x2C);  // RAMWR (Memory Write)

    uint32_t i;
    for (i = 0; i < (width * height); i++) {
        Screen_WriteSmallData(color >> 8);    // д����ֽ�
        Screen_WriteSmallData(color & 0xFF);  // д����ֽ�
    }
}


void Screen_DrawPixel(uint16_t x, uint16_t y, uint16_t color)
{
	if ((x < 0) || (x >= Screen_WIDTH) ||
		 (y < 0) || (y >= Screen_HEIGHT))	return;
	
	Screen_SetAddressWindow(x, y, x, y);
	uint8_t data[] = {color >> 8, color & 0xFF};
	Screen_Select();
	Screen_WriteData(data, sizeof(data));
	Screen_UnSelect();
}


void Screen_Fill(uint16_t xSta, uint16_t ySta, uint16_t xEnd, uint16_t yEnd, uint16_t color)
{
	if ((xEnd < 0) || (xEnd >= Screen_WIDTH) ||
		 (yEnd < 0) || (yEnd >= Screen_HEIGHT))	return;
	Screen_Select();
	uint16_t i, j;
	Screen_SetAddressWindow(xSta, ySta, xEnd, yEnd);
	for (i = ySta; i <= yEnd; i++)
		for (j = xSta; j <= xEnd; j++) {
			uint8_t data[] = {color >> 8, color & 0xFF};
			Screen_WriteData(data, sizeof(data));
		}
	Screen_UnSelect();
}


void Screen_DrawPixel_4px(uint16_t x, uint16_t y, uint16_t color)
{
	if ((x <= 0) || (x > Screen_WIDTH) ||
		 (y <= 0) || (y > Screen_HEIGHT))	return;
	Screen_Select();
	Screen_Fill(x - 1, y - 1, x + 1, y + 1, color);
	Screen_UnSelect();
}


void Screen_DrawImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t *data)
{
	if ((x >= Screen_WIDTH) || (y >= Screen_HEIGHT))
		return;
	if ((x + w - 1) >= Screen_WIDTH)
		return;
	if ((y + h - 1) >= Screen_HEIGHT)
		return;

	Screen_Select();
	Screen_SetAddressWindow(x, y, x + w - 1, y + h - 1);
	Screen_WriteData((uint8_t *)data, sizeof(uint16_t) * w * h);
	Screen_UnSelect();
}



void Screen_Test(void)
{
	Screen_Fill_Color(WHITE);
	HAL_Delay(100);

	Screen_Fill_Color(BLUE);
  HAL_Delay(100);

	Screen_Fill_Color(RED);
  HAL_Delay(100);
	
	Screen_Fill_Color(GREEN);
  HAL_Delay(100);
	
	Screen_Fill_Color(YELLOW);
  HAL_Delay(100);
}
