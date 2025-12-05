#ifndef __screen_H
#define __screen_H

#include "spi.h"
#include "fonts.h"

#define Screen_SPI_PORT hspi1
extern SPI_HandleTypeDef Screen_SPI_PORT;

/* Pin connection*/
#define Screen_RST_PORT GPIOA
#define Screen_RST_PIN  GPIO_PIN_6
#define Screen_DC_PORT  GPIOA
#define Screen_DC_PIN   GPIO_PIN_4

#ifndef CFG_NO_CS
#define Screen_CS_PORT  GPIOA
#define Screen_CS_PIN   GPIO_PIN_3
#endif

#define USING_240X240
#define Screen_ROTATION 2				//  use Normally on 240x240
#define Screen_WIDTH 240
#define Screen_HEIGHT 280
#define X_SHIFT 0
#define Y_SHIFT 20

#define WHITE       0xFFFF
#define BLACK       0x0000
#define BLUE        0x001F
#define RED         0xF800
#define MAGENTA     0xF81F
#define GREEN       0x07E0
#define CYAN        0x7FFF
#define YELLOW      0xFFE0
#define GRAY        0X8430
#define BRED        0XF81F
#define GRED        0XFFE0
#define GBLUE       0X07FF
#define BROWN       0XBC40
#define BRRED       0XFC07
#define DARKBLUE    0X01CF
#define LIGHTBLUE   0X7D7C
#define GRAYBLUE    0X5458

#define LIGHTGREEN  0X841F
#define LGRAY       0XC618
#define LGRAYBLUE   0XA651
#define LBBLUE      0X2B12

/* Control Registers and constant codes */
#define Screen_NOP     0x00
#define Screen_SWRESET 0x01
#define Screen_RDDID   0x04
#define Screen_RDDST   0x09

#define Screen_SLPIN   0x10
#define Screen_SLPOUT  0x11
#define Screen_PTLON   0x12
#define Screen_NORON   0x13

#define Screen_INVOFF  0x20
#define Screen_INVON   0x21
#define Screen_DISPOFF 0x28
#define Screen_DISPON  0x29
#define Screen_CASET   0x2A
#define Screen_RASET   0x2B
#define Screen_RAMWR   0x2C
#define Screen_RAMRD   0x2E

#define Screen_PTLAR   0x30
#define Screen_COLMOD  0x3A
#define Screen_MADCTL  0x36

/* Page Address Order ('0': Top to Bottom, '1': the opposite) */
#define Screen_MADCTL_MY  0x80
/* Column Address Order ('0': Left to Right, '1': the opposite) */
#define Screen_MADCTL_MX  0x40
/* Page/Column Order ('0' = Normal Mode, '1' = Reverse Mode) */
#define Screen_MADCTL_MV  0x20
/* Line Address Order ('0' = Screen Refresh Top to Bottom, '1' = the opposite) */
#define Screen_MADCTL_ML  0x10
/* RGB/BGR Order ('0' = RGB, '1' = BGR) */
#define Screen_MADCTL_RGB 0x00

#define Screen_RDID1   0xDA
#define Screen_RDID2   0xDB
#define Screen_RDID3   0xDC
#define Screen_RDID4   0xDD

/* Advanced options */
#define Screen_COLOR_MODE_16bit 0x55    //  RGB565 (16bit)
#define Screen_COLOR_MODE_18bit 0x66    //  RGB666 (18bit)

/* Basic operations */
#define Screen_RST_Clr() HAL_GPIO_WritePin(Screen_RST_PORT, Screen_RST_PIN, GPIO_PIN_RESET)
#define Screen_RST_Set() HAL_GPIO_WritePin(Screen_RST_PORT, Screen_RST_PIN, GPIO_PIN_SET)

#define Screen_DC_Clr() HAL_GPIO_WritePin(Screen_DC_PORT, Screen_DC_PIN, GPIO_PIN_RESET)
#define Screen_DC_Set() HAL_GPIO_WritePin(Screen_DC_PORT, Screen_DC_PIN, GPIO_PIN_SET)
#ifndef CFG_NO_CS
#define Screen_Select() HAL_GPIO_WritePin(Screen_CS_PORT, Screen_CS_PIN, GPIO_PIN_RESET)
#define Screen_UnSelect() HAL_GPIO_WritePin(Screen_CS_PORT, Screen_CS_PIN, GPIO_PIN_SET)
#else
#define Screen_Select() asm("nop")
#define Screen_UnSelect() asm("nop")
#endif

#define ABS(x) ((x) > 0 ? (x) : -(x))

/* Basic functions. */
void Screen_Init(void);
void Screen_SetRotation(uint8_t m);
void Screen_Fill_Color(uint16_t color);
void Screen_DrawPixel(uint16_t x, uint16_t y, uint16_t color);
void Screen_Fill(uint16_t xSta, uint16_t ySta, uint16_t xEnd, uint16_t yEnd, uint16_t color);
void Screen_DrawPixel_4px(uint16_t x, uint16_t y, uint16_t color);


/* Simple test function. */
void Screen_Test(void);


//�Լ�����
void Screen_Fill_Rectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color);
void Screen_DrawImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t *data);
#endif
