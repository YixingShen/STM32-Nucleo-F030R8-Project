#ifndef __BDCMOTOR_H
#define __BDCMOTOR_H

#include "stm32f0xx.h"


// 定义定时器预分频，定时器实际时钟频率为：48MHz/（BDCMOTOR_TIMx_PRESCALER+1）
#define BDCMOTOR_TIM_PRESCALER               0    // 实际时钟频率为：48MHz

// 定义定时器周期，PWM频率为：48MHz/（BDCMOTOR_TIMx_PRESCALER+1）/（BDCMOTOR_TIM_PERIOD+1）
#define BDCMOTOR_TIM_PERIOD                  2399  // PWM频率为48MHz/(2399+1)=20KHz

#define BDCMOTOR_DUTY_ZERO                   (0) // 0%占空比
#define BDCMOTOR_DUTY_FULL                   (BDCMOTOR_TIM_PERIOD-99) // 由于自举电容的存在,无法做到100%占空比

// 定义高级定时器重复计数寄存器值
// 实际PWM频率为：48MHz/（BDCMOTOR_TIMx_PRESCALER+1）/（BDCMOTOR_TIM_PERIOD+1）/（BDCMOTOR_TIM_REPETITIONCOUNTER+1）
#define BDCMOTOR_TIM_REPETITIONCOUNTER       0

#define BDCMOTOR_IDLE     0
#define BDCMOTOR_RUN      1

#define BDCMOTOR_FORWARD  0
#define BDCMOTOR_BACKWARD 1

extern __IO int16_t PWM_Duty;
extern uint8_t BDCMOTOR_state,BDCMOTOR_Dir;

void BDCMOTOR_GPIO_Init(void);
void BDCMOTOR_TIMx_Init(void);
void SetMotorSpeed(int16_t Duty);
void SetMotorDir(int16_t Dir);
void SetMotorStop(void);

#endif
