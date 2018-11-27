#ifndef __TIM_H
#define __TIM_H

#include "stm32f0xx.h"


// 定义定时器预分频，定时器实际时钟频率为：48MHz/（BDCMOTOR_TIMx_PRESCALER+1）
#define BDCMOTOR_TIM_PRESCALER               0    // 实际时钟频率为：48MHz

// 定义定时器周期，PWM频率为：48MHz/（BDCMOTOR_TIMx_PRESCALER+1）/（BDCMOTOR_TIM_PERIOD+1）
#define BDCMOTOR_TIM_PERIOD                  2399  // PWM频率为48MHz/(2399+1)=20KHz

#define BDCMOTOR_DUTY_ZERO                   (0) // 0%占空比
#define BDCMOTOR_DUTY_FULL                   (BDCMOTOR_TIM_PERIOD-24) // 由于自举电容的存在,无法做到100%占空比

// 定义高级定时器重复计数寄存器值
// 实际PWM频率为：48MHz/（BDCMOTOR_TIMx_PRESCALER+1）/（BDCMOTOR_TIM_PERIOD+1）/（BDCMOTOR_TIM_REPETITIONCOUNTER+1）
#define BDCMOTOR_TIM_REPETITIONCOUNTER       0


void TIM_config(void);
void MCO_config(void);

#endif
