#ifndef __BDCMOTOR_H
#define __BDCMOTOR_H

#include "stm32f0xx.h"


// ���嶨ʱ��Ԥ��Ƶ����ʱ��ʵ��ʱ��Ƶ��Ϊ��48MHz/��BDCMOTOR_TIMx_PRESCALER+1��
#define BDCMOTOR_TIM_PRESCALER               0    // ʵ��ʱ��Ƶ��Ϊ��48MHz

// ���嶨ʱ�����ڣ�PWMƵ��Ϊ��48MHz/��BDCMOTOR_TIMx_PRESCALER+1��/��BDCMOTOR_TIM_PERIOD+1��
#define BDCMOTOR_TIM_PERIOD                  2399  // PWMƵ��Ϊ48MHz/(2399+1)=20KHz

#define BDCMOTOR_DUTY_ZERO                   (0) // 0%ռ�ձ�
#define BDCMOTOR_DUTY_FULL                   (BDCMOTOR_TIM_PERIOD-99) // �����Ծٵ��ݵĴ���,�޷�����100%ռ�ձ�

// ����߼���ʱ���ظ������Ĵ���ֵ
// ʵ��PWMƵ��Ϊ��48MHz/��BDCMOTOR_TIMx_PRESCALER+1��/��BDCMOTOR_TIM_PERIOD+1��/��BDCMOTOR_TIM_REPETITIONCOUNTER+1��
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
