#include "stm32f1xx_hal.h"
#include "max30102.h"
#include "bsp_iic.h"
#include "bsp_delay.h"

#define MAX_BRIGHTNESS 255

iic_bus_t max30102_bus = {
    .IIC_SDA_PORT = GPIOB,
    .IIC_SDA_PIN = GPIO_PIN_11,
    .IIC_SCL_PORT = GPIOB,
    .IIC_SCL_PIN = GPIO_PIN_10
};

uint16_t fifo_red;
uint16_t fifo_ir;

static void MAX30102_WriteReg(uint8_t reg, uint8_t data)
{
    IICStart(&max30102_bus);
    IICSendByte(&max30102_bus, MAX30102_ADDRESS << 1);  // 写地址
    if (IICWaitAck(&max30102_bus) != SUCCESS) {
        IICStop(&max30102_bus);
        return;
    }
    IICSendByte(&max30102_bus, reg);                    // 寄存器地址
    IICWaitAck(&max30102_bus);
    IICSendByte(&max30102_bus, data);                   // 数据
    IICWaitAck(&max30102_bus);
    IICStop(&max30102_bus);
}

void MAX30102_Init(void)
{
    IICInit(&max30102_bus);   
    delay_ms(1); 
    
    MAX30102_WriteReg(REG_MODE_CONFIG, 0x40);   
    delay_ms(1);  
    
    MAX30102_WriteReg(REG_INTR_ENABLE_1, 0x00); // 不使用中断
    MAX30102_WriteReg(REG_INTR_ENABLE_2, 0x00);
    MAX30102_WriteReg(REG_FIFO_WR_PTR, 0x00);   // FIFO写入指针为0
    MAX30102_WriteReg(REG_OVF_COUNTER, 0x00);   // 溢出数据计数器为0
    MAX30102_WriteReg(REG_FIFO_RD_PTR, 0x00);   // FIFO读取指针为0
    MAX30102_WriteReg(REG_FIFO_CONFIG, 0x0f);   // 0x0f设置平均取样为4,当FIFO完全充满数据时,FIFO地址滚动到零并且FIFO继续填充新数据。
    MAX30102_WriteReg(REG_MODE_CONFIG, 0x03);   // SpO2模式
    MAX30102_WriteReg(REG_SPO2_CONFIG, 0x27);   // 0x27 ADC量程为8192,采样率为100/s,LED占空比118us,对应采样精度16bit
    MAX30102_WriteReg(REG_LED1_PA, 0x20);       // LED1功率
    MAX30102_WriteReg(REG_LED2_PA, 0x20);       // LED2功率
    MAX30102_WriteReg(REG_PILOT_PA, 0x7F);      // Pilot LED功率
    delay_ms(10); 
}

void MAX30102_IIC_ReadReg(uint8_t slave_ID, uint8_t RegAddress)
{
    fifo_red = 0;
    fifo_ir = 0;
    uint16_t Data1, Data2, Data3, Data4, Data5, Data6;
    
    IICStart(&max30102_bus);
    IICSendByte(&max30102_bus, slave_ID << 1);       // 写地址
    if (IICWaitAck(&max30102_bus) != SUCCESS) {
        IICStop(&max30102_bus);
        return;
    }
    IICSendByte(&max30102_bus, RegAddress);          // FIFO_DATA寄存器地址
    if (IICWaitAck(&max30102_bus) != SUCCESS) {
        IICStop(&max30102_bus);
        return;
    }
    IICStop(&max30102_bus);
    
    IICStart(&max30102_bus);
    IICSendByte(&max30102_bus, (slave_ID << 1) | 0x01);  // 读地址
    if (IICWaitAck(&max30102_bus) != SUCCESS) {
        IICStop(&max30102_bus);
        return;
    }
    
    Data1 = IICReceiveByte(&max30102_bus);
    IICSendAck(&max30102_bus);
    Data2 = IICReceiveByte(&max30102_bus);
    IICSendAck(&max30102_bus);
    Data3 = IICReceiveByte(&max30102_bus);
    IICSendAck(&max30102_bus);
    Data4 = IICReceiveByte(&max30102_bus);
    IICSendAck(&max30102_bus);
    Data5 = IICReceiveByte(&max30102_bus);
    IICSendAck(&max30102_bus);
    Data6 = IICReceiveByte(&max30102_bus);
    IICSendNotAck(&max30102_bus);  
    IICStop(&max30102_bus);
    
    Data1 <<= 14;
    fifo_red += Data1;
    Data2 <<= 6;
    fifo_red += Data2;
    Data3 >>= 2;
    fifo_red += Data3;
    Data4 <<= 14;
    fifo_ir += Data4;
    Data5 <<= 6;
    fifo_ir += Data5;
    Data6 >>= 2;
    fifo_ir += Data6;
    if (fifo_ir <= 10000) {
        fifo_ir = 0;
    }
    if (fifo_red <= 10000) {
        fifo_red = 0;
    }
}