#include "cst816.h"
#include "i2c.h"


uint8_t ChechkPush()
{
	uint8_t temp;
	HAL_I2C_Mem_Read(&hi2c1,0x15<<1,0x02,I2C_MEMADD_SIZE_8BIT,&temp,1,1000);
	
	return temp;
}

void ReadPushDate(uint8_t* date)
{
	HAL_I2C_Mem_Read(&hi2c1,0x15<<1,0x03,I2C_MEMADD_SIZE_8BIT,date,4,1000);
}