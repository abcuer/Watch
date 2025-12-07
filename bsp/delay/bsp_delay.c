#include "bsp_delay.h"
#include "stm32f1xx_hal.h"


// void delay_us(uint16_t us)
// {
//     uint32_t start = SysTick->VAL;
//     uint32_t ticks = us * (HAL_RCC_GetHCLKFreq() / 1000000);

//     while ((start - SysTick->VAL) < ticks);
// }

// void delay_us(uint16_t us)
// {
//     for(uint16_t i=0;i<us*8;i++) __NOP();
// }

void delay_us(uint32_t us)
{
	SysTick->LOAD = SYS_CLK * us;			//设置定时器重装值
	SysTick->VAL = 0x00;					//清空当前计数值
	SysTick->CTRL = 0x00000005;				//设置时钟源为HCLK，启动定时器
	while(!(SysTick->CTRL & 0x00010000));	//等待计数到0
	SysTick->CTRL = 0x00000004;				//关闭定时器
}
void delay_ms(uint32_t ms)
{
    for(uint32_t i=0;i<ms;i++) delay_us(1000);
}