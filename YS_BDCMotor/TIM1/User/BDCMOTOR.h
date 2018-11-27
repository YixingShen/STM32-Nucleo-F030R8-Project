#ifndef __TIM_H
#define __TIM_H

#include "stm32f0xx.h"


// ���嶨ʱ��Ԥ��Ƶ����ʱ��ʵ��ʱ��Ƶ��Ϊ��48MHz/��BDCMOTOR_TIMx_PRESCALER+1��
#define BDCMOTOR_TIM_PRESCALER               0    // ʵ��ʱ��Ƶ��Ϊ��48MHz

// ���嶨ʱ�����ڣ�PWMƵ��Ϊ��48MHz/��BDCMOTOR_TIMx_PRESCALER+1��/��BDCMOTOR_TIM_PERIOD+1��
#define BDCMOTOR_TIM_PERIOD                  2399  // PWMƵ��Ϊ48MHz/(2399+1)=20KHz

#define BDCMOTOR_DUTY_ZERO                   (0) // 0%ռ�ձ�
#define BDCMOTOR_DUTY_FULL                   (BDCMOTOR_TIM_PERIOD-24) // �����Ծٵ��ݵĴ���,�޷�����100%ռ�ձ�

// ����߼���ʱ���ظ������Ĵ���ֵ
// ʵ��PWMƵ��Ϊ��48MHz/��BDCMOTOR_TIMx_PRESCALER+1��/��BDCMOTOR_TIM_PERIOD+1��/��BDCMOTOR_TIM_REPETITIONCOUNTER+1��
#define BDCMOTOR_TIM_REPETITIONCOUNTER       0


void TIM_config(void);
void MCO_config(void);

#endif
