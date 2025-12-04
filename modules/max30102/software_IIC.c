#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_gpio.h"
#include "software_IIC.h"
#include "delay.h"
#include "gpio.h"


/*****************************IIC通信协议*****************************************/
void I2C_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* 1. 上电延时 (阻塞延时，OS启动前使用 HAL_Delay 较安全) */
    // 假设需要100毫秒等待OLED供电稳定
    HAL_Delay(100); 

    /* 2. 时钟使能 */
    __HAL_RCC_GPIOB_CLK_ENABLE();
    
    /* 3. 配置SCL和SDA引脚 */
    GPIO_InitStruct.Pin = GPIO_PIN_10 | GPIO_PIN_11;
    // 输出模式 (Output Mode)
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;       
    // 开漏 (Open Drain)
    GPIO_InitStruct.Pull = GPIO_PULLUP;               
    // 上拉电阻 (推荐在开漏模式下使用)
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;   
    // 速度 (F1系列通常设置为HIGH)

    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    
    /* 4. 释放SCL和SDA（确保它们被外部或内部上拉到高电平）*/
	I2C_W_SCL(1);
	I2C_W_SDA(1);
}

void I2C_W_SCL(uint8_t BitValue)
{
	/*根据BitValue的值，将SCL置高电平或者低电平*/
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, BitValue);
	/*如果单片机速度过快，可在此添加适量延时，以避免超出I2C通信的最大速度*/
	//...
}

void I2C_W_SDA(uint8_t BitValue)
{
	/*根据BitValue的值，将SDA置高电平或者低电平*/
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, BitValue);
	/*如果单片机速度过快，可在此添加适量延时，以避免超出I2C通信的最大速度*/
	//...
}
uint8_t I2C_R_SDA(void)
{
	uint8_t BitValue;
	BitValue = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_11);		//读取SDA电平
	delay_us(10);												//延时10us，防止时序频率超过要求
	return BitValue;											//返回SDA电平
}

void I2C_Start(void)
{
	I2C_W_SDA(1);		//释放SDA，确保SDA为高电平
	I2C_W_SCL(1);		//释放SCL，确保SCL为高电平
	I2C_W_SDA(0);		//在SCL高电平期间，拉低SDA，产生起始信号
	I2C_W_SCL(0);		//起始后把SCL也拉低，即为了占用总线，也为了方便总线时序的拼接
}

void I2C_Stop(void)
{
	I2C_W_SDA(0);		//拉低SDA，确保SDA为低电平
	I2C_W_SCL(1);		//释放SCL，使SCL呈现高电平
	I2C_W_SDA(1);		//在SCL高电平期间，释放SDA，产生终止信号
}
void I2C_SendByte(uint8_t Byte)
{
	uint8_t i;
	
	/*循环8次，主机依次发送数据的每一位*/
	for (i = 0; i < 8; i++)
	{
		/*使用掩码的方式取出Byte的指定一位数据并写入到SDA线*/
		/*两个!的作用是，让所有非零的值变为1*/
		I2C_W_SDA(!!(Byte & (0x80 >> i)));
		I2C_W_SCL(1);	//释放SCL，从机在SCL高电平期间读取SDA
		I2C_W_SCL(0);	//拉低SCL，主机开始发送下一位数据
	}
	
}
uint8_t I2C_ReceiveByte(void)
{
	uint8_t i, Byte = 0x00;					//定义接收的数据，并赋初值0x00
	I2C_W_SDA(1);							//接收前，主机先确保释放SDA，避免干扰从机的数据发送
	for (i = 0; i < 8; i ++)				//循环8次，主机依次接收数据的每一位
	{
		I2C_W_SCL(1);						//释放SCL，主机机在SCL高电平期间读取SDA
		if (I2C_R_SDA() == 1){Byte |= (0x80 >> i);}	//读取SDA数据，并存储到Byte变量
														//当SDA为1时，置变量指定位为1，当SDA为0时，不做处理，指定位为默认的初值0
		I2C_W_SCL(0);						//拉低SCL，从机在SCL低电平期间写入SDA
	}
	return Byte;							//返回接收到的一个字节数据
}

void I2C_SendAck(uint8_t AckBit)
{
	I2C_W_SDA(AckBit);					//主机把应答位数据放到SDA线
	I2C_W_SCL(1);							//释放SCL，从机在SCL高电平期间，读取应答位
	I2C_W_SCL(0);							//拉低SCL，开始下一个时序模块
}

uint8_t I2C_ReceiveAck(void)
{
	uint8_t AckBit;							//定义应答位变量
	I2C_W_SDA(1);							//接收前，主机先确保释放SDA，避免干扰从机的数据发送
	I2C_W_SCL(1);							//释放SCL，主机机在SCL高电平期间读取SDA
	AckBit = I2C_R_SDA();					//将应答位存储到变量里
	I2C_W_SCL(0);							//拉低SCL，开始下一个时序模块
	return AckBit;							//返回定义应答位变量
}

void I2C_WriteReg(uint8_t slave_ID,uint8_t RegAddress, uint8_t Data)
{
	I2C_Start();						//I2C起始
	I2C_SendByte(slave_ID);		//发送从机地址，读写位为0，表示即将写入
	I2C_ReceiveAck();					//接收应答
	I2C_SendByte(RegAddress);			//发送寄存器地址
	I2C_ReceiveAck();					//接收应答
	I2C_SendByte(Data);				//发送要写入寄存器的数据
	I2C_ReceiveAck();					//接收应答
	I2C_Stop();						//I2C终止
}
uint8_t I2C_ReadReg(uint8_t slave_ID,uint8_t RegAddress)  //读取一个字节
{
	uint8_t Data;
	
	I2C_Start();						//I2C起始
	I2C_SendByte(slave_ID);	//发送从机地址，读写位为0，表示即将写入
	I2C_ReceiveAck();					//接收应答
	I2C_SendByte(RegAddress);			//发送寄存器地址
	I2C_ReceiveAck();					//接收应答
	
	I2C_Start();						//I2C重复起始
	I2C_SendByte(slave_ID | 0x01);	//发送从机地址，读写位为1，表示即将读取
	I2C_ReceiveAck();					//接收应答
	Data = I2C_ReceiveByte();			//接收指定寄存器的数据
	I2C_SendAck(1);					//发送应答，给从机非应答，终止从机的数据输出
	I2C_Stop();						//I2C终止
	
	return Data;
}


/****************************************************IIC通信协议*******************************************/


