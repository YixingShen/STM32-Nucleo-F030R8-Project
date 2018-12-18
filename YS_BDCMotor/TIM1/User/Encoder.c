#include "Encoder.h"

int16_t OverflowCount = 0;//定时器溢出次数

void ENCODER_GPIO_Init(void)
{
    /*PA6: TIM3_CH1*/
    RCC->AHBENR |= ENCODER_TIM_CH1_GPIO_RCC; //打开GPIO时钟
    ENCODER_TIM_CH1_PORT->MODER |= GPIO_MODER_MODER6_1; //复用功能
    ENCODER_TIM_CH1_PORT->OTYPER &= (~GPIO_OTYPER_OT_6); //推挽输出
    ENCODER_TIM_CH1_PORT->OSPEEDR |= GPIO_OSPEEDR_OSPEEDR6; //高速
    ENCODER_TIM_CH1_PORT->PUPDR |= GPIO_PUPDR_PUPDR6_0; //上拉
    ENCODER_TIM_CH1_PORT->AFR[0] |= (1<<24);//AF1
    
    /*PA7: TIM3_CH2*/
    RCC->AHBENR |= ENCODER_TIM_CH2_GPIO_RCC; //打开GPIO时钟
    ENCODER_TIM_CH2_PORT->MODER |= GPIO_MODER_MODER7_1; //复用功能
    ENCODER_TIM_CH2_PORT->OTYPER &= (~GPIO_OTYPER_OT_7); //推挽输出
    ENCODER_TIM_CH2_PORT->OSPEEDR |= GPIO_OSPEEDR_OSPEEDR7; //高速
    ENCODER_TIM_CH2_PORT->PUPDR |= GPIO_PUPDR_PUPDR7_0; //上拉    
    ENCODER_TIM_CH2_PORT->AFR[0] |= (1<<28);//AF1
}

void ENCODER_TIMx_Init(void)
{
    RCC->APB1ENR |= ENCODER_TIM_RCC; //使能外设时钟
    
    /* 定时器基本环境配置 */
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
    //TIM_ClearFlag(ENCODER_TIMx, TIM_FLAG_Update); // 清除更新中断标志位
    TIM_ClearITPendingBit(ENCODER_TIMx, TIM_IT_Update);
    TIM_UpdateRequestConfig(ENCODER_TIMx, TIM_UpdateSource_Regular); // 仅允许计数器溢出才产生更新中断
    TIM_ITConfig(ENCODER_TIMx, TIM_IT_Update, ENABLE); // 使能更新中断

    NVIC_SetPriority(ENCODER_TIM_IRQn, 0);
    NVIC_EnableIRQ(ENCODER_TIM_IRQn);     
    
    TIM_Cmd(ENCODER_TIMx, ENABLE);
}


/**
  * 函数功能: 中断服务函数
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 记录溢出次数
  */
void ENCODER_TIM_IRQHANDLER(void)
{
    /* TIM Update event */
    //if(TIM_GetFlagStatus(ENCODER_TIMx, TIM_FLAG_Update) != RESET)
    if((ENCODER_TIMx->SR & TIM_SR_UIF) !=RESET)
    {
        ENCODER_TIMx->SR &= ~TIM_SR_UIF; // 清除更新中断标志位
        
        if(ENCODER_TIMx->CR1 & TIM_CR1_DIR)
            OverflowCount--;       //向下计数溢出
        else
            OverflowCount++;       //向上计数溢出
    }
}

