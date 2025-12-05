#include "max30102.h"
#include "software_IIC.h"
#include "max30102_algorithm.h"
#include "bsp_delay.h"

uint16_t fifo_red;
uint16_t fifo_ir;
void MAX30102_Init(void)
{
	I2C_GPIO_Init();									//先初始化底层的I2C
	
	/*MAX30102寄存器初始化，需要对照MAX30102手册的寄存器描述配置，此处仅配置了部分重要的寄存器*/
	I2C_WriteReg(MAX30102_ADDRESS,REG_MODE_CONFIG, 0x40);   //将RESET位设置为1，所有配置、阈值和数据寄存器通过上电复位复位复位到其上电状态。
	I2C_WriteReg(MAX30102_ADDRESS,REG_INTR_ENABLE_1, 0x00);	//不使用中断	
	I2C_WriteReg(MAX30102_ADDRESS,REG_INTR_ENABLE_2, 0x00);		
	I2C_WriteReg(MAX30102_ADDRESS,REG_FIFO_WR_PTR,0x00);	//FIFO写入指针为0		
	I2C_WriteReg(MAX30102_ADDRESS,REG_OVF_COUNTER,0x00);    //溢出数据计数器为0
	I2C_WriteReg(MAX30102_ADDRESS,REG_FIFO_RD_PTR,0x00);    //FIFO读取指针为0
	I2C_WriteReg(MAX30102_ADDRESS,REG_FIFO_CONFIG,0x0f);    //0x0f设置平均取样为4,当FIFO完全充满数据时,FIFO地址滚动到零并且FIFO继续填充新数据。
	I2C_WriteReg(MAX30102_ADDRESS,REG_MODE_CONFIG,0x03);    //SpO2模式
	I2C_WriteReg(MAX30102_ADDRESS,REG_SPO2_CONFIG,0x27);    //0x45ADC量程为8192,采样率为100/s,LED占空比118us,对应采样精度16bit
	I2C_WriteReg(MAX30102_ADDRESS,REG_LED1_PA,0x20);
	I2C_WriteReg(MAX30102_ADDRESS,REG_LED2_PA,0x20);
	I2C_WriteReg(MAX30102_ADDRESS,REG_PILOT_PA,0x7F);
		
}

void MAX30102_IIC_ReadReg(uint8_t slave_ID,uint8_t RegAddress)  //读取6个字节
{
    fifo_red=0;
    fifo_ir=0;
	uint16_t Data1,Data2,Data3,Data4,Data5,Data6;
	
	I2C_Start();						//I2C起始
	I2C_SendByte(slave_ID);	//发送从机地址，读写位为0，表示即将写入
	I2C_ReceiveAck();					//接收应答
	I2C_SendByte(RegAddress);			//发送寄存器地址
	I2C_ReceiveAck();					//接收应答
	
	I2C_Start();						//I2C重复起始
	I2C_SendByte(slave_ID | 0x01);	//发送从机地址，读写位为1，表示即将读取
	I2C_ReceiveAck();					//接收应答
	Data1 = I2C_ReceiveByte();			//接收指定寄存器的数据
	I2C_SendAck(0);					//发送应答，给从机非应答，终止从机的数据输出
	Data2 = I2C_ReceiveByte();
	I2C_SendAck(0);	
	Data3 = I2C_ReceiveByte();
	I2C_SendAck(0);
	Data4 = I2C_ReceiveByte();			//接收指定寄存器的数据
	I2C_SendAck(0);					//发送应答，给从机非应答，终止从机的数据输出
	Data5 = I2C_ReceiveByte();
	I2C_SendAck(0);	
	Data6 = I2C_ReceiveByte();
	I2C_SendAck(1);
	I2C_Stop();	
	//I2C终止
	Data1 <<= 14;
    fifo_red+=Data1;
	Data2 <<= 6;
    fifo_red+=Data2;
	Data3 >>= 2;
    fifo_red+=Data3;
	
	Data4 <<= 14;
    fifo_ir+=Data4;
	Data5 <<= 6;
    fifo_ir+=Data5;
	Data6 >>= 2;
    fifo_ir+=Data6;
	
	if(fifo_ir<=10000)
	{
		fifo_ir=0;
	}
	if(fifo_red<=10000)
	{
		fifo_red=0;
	}
}