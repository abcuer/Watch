#include "MPU6050.h"

// 定义MPU6050的总线实例
iic_bus_t mpu6050_bus = {
    .IIC_SDA_PORT = GPIOB,
    .IIC_SDA_PIN = GPIO_PIN_11,
    .IIC_SCL_PORT = GPIOB,
    .IIC_SCL_PIN = GPIO_PIN_10
};

// 写多个字节
uint8_t mpu6050_write(uint8_t addr, uint8_t reg, uint8_t len, uint8_t* buf)
{
    IICStart(&mpu6050_bus);
    
    // 发送器件地址（写模式）
    IICSendByte(&mpu6050_bus, addr << 1);
    if (IICWaitAck(&mpu6050_bus) != SUCCESS) {
        IICStop(&mpu6050_bus);
        return 1;  // 失败
    }
    
    // 发送寄存器地址
    IICSendByte(&mpu6050_bus, reg);
    if (IICWaitAck(&mpu6050_bus) != SUCCESS) {
        IICStop(&mpu6050_bus);
        return 1;
    }
    
    // 发送数据
    for (uint8_t i = 0; i < len; i++) {
        IICSendByte(&mpu6050_bus, buf[i]);
        if (IICWaitAck(&mpu6050_bus) != SUCCESS) {
            IICStop(&mpu6050_bus);
            return 1;
        }
    }
    
    IICStop(&mpu6050_bus);
    return 0;  // 成功
}

// 读多个字节
uint8_t mpu6050_read(uint8_t addr, uint8_t reg, uint8_t len, uint8_t* buf)
{
    IICStart(&mpu6050_bus);
    
    // 发送器件地址（写模式）- 写寄存器地址
    IICSendByte(&mpu6050_bus, addr << 1);
    if (IICWaitAck(&mpu6050_bus) != SUCCESS) {
        IICStop(&mpu6050_bus);
        return 1;
    }
    
    // 发送寄存器地址
    IICSendByte(&mpu6050_bus, reg);
    if (IICWaitAck(&mpu6050_bus) != SUCCESS) {
        IICStop(&mpu6050_bus);
        return 1;
    }
    
    // 重复开始信号，切换到读模式
    IICStart(&mpu6050_bus);
    
    // 发送器件地址（读模式）
    IICSendByte(&mpu6050_bus, (addr << 1) | 0x01);
    if (IICWaitAck(&mpu6050_bus) != SUCCESS) {
        IICStop(&mpu6050_bus);
        return 1;
    }
    
    // 读取数据
    for (uint8_t i = 0; i < len; i++) {
        if (i < len - 1) {
            buf[i] = IICReceiveByte(&mpu6050_bus);
            IICSendAck(&mpu6050_bus);  // 发送ACK
        } else {
            buf[i] = IICReceiveByte(&mpu6050_bus);
            IICSendNotAck(&mpu6050_bus);  // 最后一个字节发送NACK
        }
    }
    
    IICStop(&mpu6050_bus);
    return 0;  // 成功
}

// 写单个寄存器
void mpu6050_write_reg(uint8_t reg, uint8_t dat)
{
    mpu6050_write(MPU_ADDR, reg, 1, &dat);
}

// 读单个寄存器
uint8_t mpu6050_read_reg(uint8_t reg)
{
    uint8_t dat;
    mpu6050_read(MPU_ADDR, reg, 1, &dat);
    return dat;
}

// 设置陀螺仪满量程范围
uint8_t MPU_Set_Gyro_Fsr(uint8_t fsr)
{
    mpu6050_write_reg(GYRO_CONFIG, fsr << 3);
    return 0;
}

// 设置加速度计满量程范围
uint8_t MPU_Set_Accel_Fsr(uint8_t fsr)
{
    mpu6050_write_reg(ACCEL_CONFIG, fsr << 3);
    return 0;
}

// 设置数字低通滤波器
uint8_t MPU_Set_LPF(uint16_t lpf)
{
    uint8_t data = 0;
    if (lpf >= 188) data = 1;
    else if (lpf >= 98) data = 2;
    else if (lpf >= 42) data = 3;
    else if (lpf >= 20) data = 4;
    else if (lpf >= 10) data = 5;
    else data = 6;
    
    mpu6050_write_reg(MPU_CFG_REG, data);
    return 0;
}

// 设置采样率
uint8_t MPU_Set_Rate(uint16_t rate)
{
    uint8_t data;
    if (rate > 1000) rate = 1000;
    if (rate < 4) rate = 4;
    data = 1000 / rate - 1;
    mpu6050_write_reg(MPU_SAMPLE_RATE_REG, data);
    return MPU_Set_LPF(rate / 2);  // 自动设置LPF为采样率的一半
}

// MPU6050初始化
void MPU_Init(void)
{ 
    uint8_t res;
    
    // 初始化I2C总线
    IICInit(&mpu6050_bus);
    
    // 复位MPU6050
    mpu6050_write_reg(PWR_MGMT_1, 0x80);
    delay_ms(100);
    
    // 唤醒MPU6050
    mpu6050_write_reg(PWR_MGMT_1, 0x00);
    
    // 设置陀螺仪和加速度计量程
    MPU_Set_Gyro_Fsr(3);     // ±2000dps
    MPU_Set_Accel_Fsr(0);    // ±2g
    
    // 设置采样率
    MPU_Set_Rate(200);       // 200Hz采样率
    
    // 配置其他寄存器
    mpu6050_write_reg(MPU_INT_EN_REG, 0x00);      // 关闭所有中断
    mpu6050_write_reg(MPU_USER_CTRL_REG, 0x00);   // I2C主模式关闭
    mpu6050_write_reg(MPU_FIFO_EN_REG, 0x00);     // 关闭FIFO
    mpu6050_write_reg(MPU_INTBP_CFG_REG, 0x80);   // INT引脚低电平有效
    
    // 读取器件ID
    res = mpu6050_read_reg(MPU_DEVICE_ID_REG);
    
    if ((res & 0x7E) == (MPU_ADDR << 1))  // 器件ID正确
    {
        mpu6050_write_reg(PWR_MGMT_1, 0x01);  // 设置CLKSEL,PLL X轴为参考
        mpu6050_write_reg(PWR_MGMT_2, 0x00);  // 加速度与陀螺仪都工作
        MPU_Set_Rate(100);                    // 设置采样率为100Hz
    }
}

// 获取温度值（摄氏度）
float MPU_Get_Temperature(void)
{
    uint8_t buf[2];
    int16_t raw;
    float temp;
    
    mpu6050_read(MPU_ADDR, TEMP_OUT_H, 2, buf);
    raw = ((uint16_t)buf[0] << 8) | buf[1];
    temp = 36.53f + ((float)raw) / 340.0f;
    
    return temp;
}

// 获取陀螺仪原始数据
uint8_t MPU_Get_Gyroscope(int16_t *gx, int16_t *gy, int16_t *gz)
{
    uint8_t buf[6];
    uint8_t res;
    
    res = mpu6050_read(MPU_ADDR, GYRO_XOUT_H, 6, buf);
    if (res == 0)
    {
        *gx = ((uint16_t)buf[0] << 8) | buf[1];
        *gy = ((uint16_t)buf[2] << 8) | buf[3];
        *gz = ((uint16_t)buf[4] << 8) | buf[5];
    }
    
    return res;
}

// 获取加速度计原始数据
uint8_t MPU_Get_Accelerometer(int16_t *ax, int16_t *ay, int16_t *az)
{
    uint8_t buf[6];
    uint8_t res;
    
    res = mpu6050_read(MPU_ADDR, ACCEL_XOUT_H, 6, buf);
    if (res == 0)
    {
        *ax = ((uint16_t)buf[0] << 8) | buf[1];
        *ay = ((uint16_t)buf[2] << 8) | buf[3];
        *az = ((uint16_t)buf[4] << 8) | buf[5];
    }
    
    return res;
}

// 可选：一次性读取所有6轴数据
uint8_t MPU_Get_6Axis(int16_t *ax, int16_t *ay, int16_t *az, int16_t *gx, int16_t *gy, int16_t *gz)
{
    uint8_t buf[14];  // 6轴数据 + 温度 = 14字节
    uint8_t res;
    
    res = mpu6050_read(MPU_ADDR, ACCEL_XOUT_H, 14, buf);
    if (res == 0)
    {
        // 加速度计数据
        *ax = ((uint16_t)buf[0] << 8) | buf[1];
        *ay = ((uint16_t)buf[2] << 8) | buf[3];
        *az = ((uint16_t)buf[4] << 8) | buf[5];
        
        // 温度数据（跳过）
        // temp_raw = ((uint16_t)buf[6] << 8) | buf[7];
        
        // 陀螺仪数据
        *gx = ((uint16_t)buf[8] << 8) | buf[9];
        *gy = ((uint16_t)buf[10] << 8) | buf[11];
        *gz = ((uint16_t)buf[12] << 8) | buf[13];
    }
    
    return res;
}

// 获取俯仰角(pitch)和横滚角(roll)
// 使用加速度计数据计算，单位：度(°)
// pitch: 俯仰角（绕X轴旋转），范围：-90° ~ +90°
// roll:  横滚角（绕Y轴旋转），范围：-180° ~ +180°
void MPU_Get_Angle(MPU_t *mpu)
{
    // 读取加速度计原始数据到结构体
    if (MPU_Get_Accelerometer(&mpu->ax, &mpu->ay, &mpu->az) != 0) {
        mpu->pitch = 0.0f;
        mpu->roll = 0.0f;
        return;
    }
    
    // 直接使用原始值计算，避免中间变量
    float denom = sqrtf((mpu->ay * mpu->ay) + (mpu->az * mpu->az));
    if (denom != 0.0f) {
        mpu->pitch = atan2f(-mpu->ax, denom) * (1.0f / 16384.0f * 57.29578f);
    } else {
        mpu->pitch = 0.0f;
    }
    
    denom = sqrtf((mpu->ax * mpu->ax) + (mpu->az * mpu->az));
    if (denom != 0.0f) {
        mpu->roll = atan2f(mpu->ay, denom) * (1.0f / 16384.0f * 57.29578f);
    } else {
        mpu->roll = 0.0f;
    }
}

// 使用互补滤波计算角度，结果直接存入MPU_t结构体
// 参数：mpu - 结构体指针，dt - 时间间隔(秒)，alpha - 滤波系数(0.96-0.98)
void MPU_Get_Angle_Complementary(MPU_t *mpu, float dt, float alpha)
{
    static float pitch_angle = 0.0f;
    static float roll_angle = 0.0f;
    
    float accel_pitch, accel_roll;
    
    if (MPU_Get_Accelerometer(&mpu->ax, &mpu->ay, &mpu->az) != 0 ||
        MPU_Get_Gyroscope(&mpu->gx, &mpu->gy, &mpu->gz) != 0) {
        // 读取失败，使用上次的角度值
        mpu->pitch = pitch_angle;
        mpu->roll = roll_angle;
        return;
    }
    float denom = sqrtf((mpu->ay * mpu->ay) + (mpu->az * mpu->az));
    if (denom > 100) {  // 避免除零和小值
        accel_pitch = atan2f(-mpu->ax, denom) * 0.0174532925f; // 弧度
    } else {
        accel_pitch = pitch_angle * 0.0174532925f;
    }
    
    denom = sqrtf((mpu->ax * mpu->ax) + (mpu->az * mpu->az));
    if (denom > 100) {
        accel_roll = atan2f(mpu->ay, denom) * 0.0174532925f;
    } else {
        accel_roll = roll_angle * 0.0174532925f;
    }
    
    // 3. 转换为角度（度）
    accel_pitch *= 57.29578f;  // 180/π
    accel_roll *= 57.29578f;
    
    // 4. 陀螺仪积分（直接使用原始值）
    // ±2000dps量程：16.4 LSB/(°/s)
    float gyro_pitch = -(mpu->gy / 16.4f) * dt;  // 注意负号
    float gyro_roll = (mpu->gx / 16.4f) * dt;
    
    // 5. 互补滤波
    pitch_angle = alpha * (pitch_angle + gyro_pitch) + (1.0f - alpha) * accel_pitch;
    roll_angle = alpha * (roll_angle + gyro_roll) + (1.0f - alpha) * accel_roll;
    
    // 6. 存入结构体
    mpu->pitch = pitch_angle;
    mpu->roll = roll_angle;
}

// 高效版本：整合数据读取和角度计算
void MPU_Update_Angles(MPU_t *mpu, float dt, float alpha)
{
    static float pitch = 0.0f, roll = 0.0f;
    
    // 读取数据到结构体
    MPU_Get_Accelerometer(&mpu->ax, &mpu->ay, &mpu->az);
    MPU_Get_Gyroscope(&mpu->gx, &mpu->gy, &mpu->gz);
    
    // 计算角度（最小化计算）
    float sum_ay2_az2 = (mpu->ay * mpu->ay) + (mpu->az * mpu->az);
    float sum_ax2_az2 = (mpu->ax * mpu->ax) + (mpu->az * mpu->az);
    
    if (sum_ay2_az2 > 10000) {  // 约0.6g²
        float acc_pitch = atan2f(-mpu->ax, sqrtf(sum_ay2_az2)) * 57.29578f;
        float gyro_pitch = -(mpu->gy / 16.4f) * dt;
        pitch = alpha * (pitch + gyro_pitch) + (1.0f - alpha) * acc_pitch;
    }
    
    if (sum_ax2_az2 > 10000) {
        float acc_roll = atan2f(mpu->ay, sqrtf(sum_ax2_az2)) * 57.29578f;
        float gyro_roll = (mpu->gx / 16.4f) * dt;
        roll = alpha * (roll + gyro_roll) + (1.0f - alpha) * acc_roll;
    }
    
    // 更新结构体
    mpu->pitch = pitch;
    mpu->roll = roll;
}