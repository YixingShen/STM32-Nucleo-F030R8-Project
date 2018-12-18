#include "Encoder.h"

int16_t OverflowCount = 0;//定时器溢出次数

void ENCODER_GPIO_Init(void)
{
    /*****************************************
        PIN22:PA6--NetCN5_5
        PIN23:PA7--NetCN5_4
    *****************************************/
    
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
    //向上计数
    ENCODER_TIMx->CR1 = 0;
    ENCODER_TIMx->ARR = ENCODER_TIM_PERIOD;
    ENCODER_TIMx->PSC = ENCODER_TIM_PRESCALER;
    ENCODER_TIMx->CNT = 0;
    ENCODER_TIMx->EGR = TIM_EGR_UG;
    
    //编码器模式3
    ENCODER_TIMx->SMCR = TIM_SMCR_SMS_0 | TIM_SMCR_SMS_1;
    ENCODER_TIMx->CCMR1 = TIM_CCMR1_CC1S_0 | TIM_CCMR1_CC2S_0;
    ENCODER_TIMx->CCER = TIM_CCER_CC2P | TIM_CCER_CC2E | TIM_CCER_CC1P | TIM_CCER_CC1E;

    ENCODER_TIMx->SR &= (~TIM_SR_UIF); // 清除更新中断标志位
    ENCODER_TIMx->CR1 |= TIM_CR1_URS; // 仅允许计数器溢出才产生更新中断
    ENCODER_TIMx->DIER = TIM_DIER_UIE; // 使能更新中断

    NVIC_SetPriority(ENCODER_TIM_IRQn, 0);
    NVIC_EnableIRQ(ENCODER_TIM_IRQn);     
    
    ENCODER_TIMx->CR1 |= TIM_CR1_CEN;
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

