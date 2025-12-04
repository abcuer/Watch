#include "delay.h"



// void delay_us(uint16_t us)
// {
//     uint32_t start = SysTick->VAL;
//     uint32_t ticks = us * (HAL_RCC_GetHCLKFreq() / 1000000);

//     while ((start - SysTick->VAL) < ticks);
// }

void delay_us(uint16_t us)
{
    for(uint16_t i=0;i<us*8;i++) __NOP();
}

void delay_ms(uint16_t ms)
{
    for(uint16_t i=0;i<ms;i++) delay_us(1000);
}