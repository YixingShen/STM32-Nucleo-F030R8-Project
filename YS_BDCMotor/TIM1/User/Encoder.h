#ifndef __ENCODER_H
#define __ENCODER_H

#include "stm32f0xx.h"

//PA6: TIM3_CH1  PA7: TIM3_CH2
#define ENCODER_TIMx                        TIM3
#define ENCODER_TIM_RCC                     RCC_APB1ENR_TIM3EN

#define ENCODER_TIM_CH1_GPIO_RCC            RCC_AHBENR_GPIOAEN
#define ENCODER_TIM_CH1_PORT                GPIOA
#define ENCODER_TIM_CH1_PIN      			GPIO_Pin_6

#define ENCODER_TIM_CH2_GPIO_RCC            RCC_AHBENR_GPIOAEN
#define ENCODER_TIM_CH2_PORT                GPIOA
#define ENCODER_TIM_CH2_PIN      			GPIO_Pin_7

#define TIM_ENCODERMODE_TIx                 TIM_EncoderMode_TI12

#define ENCODER_TIM_IRQn                    TIM3_IRQn
#define ENCODER_TIM_IRQHANDLER              TIM3_IRQHandler

// 定义定时器预分频，定时器实际时钟频率为：8MHz/（ENCODER_TIMx_PRESCALER+1）
#define ENCODER_TIM_PRESCALER               0  

// 定义定时器周期，当定时器开始计数到ENCODER_TIMx_PERIOD值时更新定时器并生成对应事件和中断
#define ENCODER_TIM_PERIOD                  0xFFFF

extern int16_t OverflowCount ;//定时器溢出次数

void ENCODER_GPIO_Init(void);
void ENCODER_TIMx_Init(void);

#endif
