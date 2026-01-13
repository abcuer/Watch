#include "bsp_timer.h"
#include "lvgl.h"
/**
 * @brief   HAL库基本定时器超时中断回调函数
 * @param   无
 * @retval  无
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{   
    if (htim->Instance == TIM4)
    {
        // HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13); // 翻转PC13引脚电平
        lv_tick_inc(1);
    }
}