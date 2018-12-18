#include "Encoder.h"

int16_t OverflowCount = 0;//��ʱ���������

void ENCODER_GPIO_Init(void)
{
    /*PA6: TIM3_CH1*/
    RCC->AHBENR |= ENCODER_TIM_CH1_GPIO_RCC; //��GPIOʱ��
    ENCODER_TIM_CH1_PORT->MODER |= GPIO_MODER_MODER6_1; //���ù���
    ENCODER_TIM_CH1_PORT->OTYPER &= (~GPIO_OTYPER_OT_6); //�������
    ENCODER_TIM_CH1_PORT->OSPEEDR |= GPIO_OSPEEDR_OSPEEDR6; //����
    ENCODER_TIM_CH1_PORT->PUPDR |= GPIO_PUPDR_PUPDR6_0; //����
    ENCODER_TIM_CH1_PORT->AFR[0] |= (1<<24);//AF1
    
    /*PA7: TIM3_CH2*/
    RCC->AHBENR |= ENCODER_TIM_CH2_GPIO_RCC; //��GPIOʱ��
    ENCODER_TIM_CH2_PORT->MODER |= GPIO_MODER_MODER7_1; //���ù���
    ENCODER_TIM_CH2_PORT->OTYPER &= (~GPIO_OTYPER_OT_7); //�������
    ENCODER_TIM_CH2_PORT->OSPEEDR |= GPIO_OSPEEDR_OSPEEDR7; //����
    ENCODER_TIM_CH2_PORT->PUPDR |= GPIO_PUPDR_PUPDR7_0; //����    
    ENCODER_TIM_CH2_PORT->AFR[0] |= (1<<28);//AF1
}

void ENCODER_TIMx_Init(void)
{
    RCC->APB1ENR |= ENCODER_TIM_RCC; //ʹ������ʱ��
    
    /* ��ʱ�������������� */
    /* Time base configuration */
    TIM_TimeBaseStructure.TIM_Period = ENCODER_TIM_PERIOD;
    TIM_TimeBaseStructure.TIM_Prescaler = ENCODER_TIM_PRESCALER;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(ENCODER_TIMx, &TIM_TimeBaseStructure);    
    
    TIM_SetCounter(ENCODER_TIMx, 0);
    
#if 1 
    TIM_EncoderInterfaceConfig(ENCODER_TIMx, TIM_ENCODERMODE_TIx, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
    ENCODER_TIMx->CCER |= TIM_CCER_CC1E | TIM_CCER_CC2E;
#else
    ENCODER_TIMx->SMCR |= TIM_SMCR_SMS_0 | TIM_SMCR_SMS_1;
    
    sEncoderConfig.TIM_Channel = TIM_Channel_1;
    sEncoderConfig.TIM_ICFilter = 0;
    sEncoderConfig.TIM_ICPolarity = TIM_ICPolarity_Rising;
    sEncoderConfig.TIM_ICPrescaler = TIM_ICPSC_DIV1;
    sEncoderConfig.TIM_ICSelection = TIM_ICSelection_DirectTI;
    TIM_ICInit(ENCODER_TIMx, &sEncoderConfig);
    
    sEncoderConfig.TIM_Channel = TIM_Channel_2;
    TIM_ICInit(ENCODER_TIMx, &sEncoderConfig);
#endif
    //TIM_ClearFlag(ENCODER_TIMx, TIM_FLAG_Update); // ��������жϱ�־λ
    TIM_ClearITPendingBit(ENCODER_TIMx, TIM_IT_Update);
    TIM_UpdateRequestConfig(ENCODER_TIMx, TIM_UpdateSource_Regular); // ���������������Ų��������ж�
    TIM_ITConfig(ENCODER_TIMx, TIM_IT_Update, ENABLE); // ʹ�ܸ����ж�

    NVIC_SetPriority(ENCODER_TIM_IRQn, 0);
    NVIC_EnableIRQ(ENCODER_TIM_IRQn);     
    
    TIM_Cmd(ENCODER_TIMx, ENABLE);
}


/**
  * ��������: �жϷ�����
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��¼�������
  */
void ENCODER_TIM_IRQHANDLER(void)
{
    /* TIM Update event */
    //if(TIM_GetFlagStatus(ENCODER_TIMx, TIM_FLAG_Update) != RESET)
    if((ENCODER_TIMx->SR & TIM_SR_UIF) !=RESET)
    {
        ENCODER_TIMx->SR &= ~TIM_SR_UIF; // ��������жϱ�־λ
        
        if(ENCODER_TIMx->CR1 & TIM_CR1_DIR)
            OverflowCount--;       //���¼������
        else
            OverflowCount++;       //���ϼ������
    }
}

