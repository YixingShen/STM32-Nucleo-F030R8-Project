#include "BDCMOTOR.h"

uint8_t BDCMOTOR_state,BDCMOTOR_Dir;
__IO int16_t PWM_Duty=BDCMOTOR_DUTY_ZERO; // 占空比：PWM_Duty/BDCMOTOR_TIM_PERIOD*100%

void BDCMOTOR_GPIO_Init(void)
{
    /*****************************************
        PIN41:PA8--TIM1_CH1
        PIN34:PB13(NetCN10_30)--TIM1_CH1N
        PIN42:PA9--SHUTDOWN
    *****************************************/
	//PA8
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;  //打开Port A时钟
	GPIOA->MODER &= (~GPIO_MODER_MODER8);
	GPIOA->MODER |= GPIO_MODER_MODER8_1; //复用功能
	GPIOA->OTYPER &= (~GPIO_OTYPER_OT_8); //推挽输出
    GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEEDR8; //设置引脚速度为50MHZ  
	GPIOA->AFR[1] &= ~GPIO_AFRH_AFRH0;//复用功能AF2：TIM1_CH1
    GPIOA->AFR[1] |= 0x00000002U;//
    
	//PB13
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;  //打开Port B时钟
	GPIOB->MODER &= (~GPIO_MODER_MODER13);
	GPIOB->MODER |= GPIO_MODER_MODER13_1; //复用功能
	GPIOB->OTYPER &= (~GPIO_OTYPER_OT_13); //推挽输出
    GPIOB->OSPEEDR |= GPIO_OSPEEDR_OSPEEDR13; //设置引脚速度为50MHZ  
	GPIOB->AFR[1] &= ~GPIO_AFRH_AFRH5;//复用功能AF2：TIM1_CH1N
    GPIOB->AFR[1] |= 0x00200000U;//

	//PA9
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;  //打开Port A时钟
	GPIOA->MODER &= (~GPIO_MODER_MODER9);
	GPIOA->MODER |= GPIO_MODER_MODER9_0; //输出
	GPIOA->OTYPER &= (~GPIO_OTYPER_OT_9); //推挽输出
    GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEEDR8; //设置引脚速度为50MHZ  
    GPIOA->ODR &= (~GPIO_ODR_9); //输出低电平
}

void BDCMOTOR_TIMx_Init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN; //使能TIM1时钟

    /* Disable slave mode to clock the prescaler directly with the internal clock */
    TIM1->SMCR &= ~TIM_SMCR_SMS; //使用内部时钟
    
    /* 定时器基本环境配置 */
    TIM1->PSC = BDCMOTOR_TIM_PRESCALER; //CK_CNT = fCK_PSC/(PSC[15:0] + 1)
    TIM1->ARR = BDCMOTOR_TIM_PERIOD;
    TIM1->RCR = BDCMOTOR_TIM_REPETITIONCOUNTER;
    TIM1->CR1 &= ~(TIM_CR1_DIR | TIM_CR1_CMS | TIM_CR1_CKD); //向上计数，边沿对齐，死区发生器和数字滤波使用的采样时钟tDTS和定时器时钟(CK_INT)不分频
    /* Generate an update event to reload the Prescaler 
       and the repetition counter(only for TIM1 and TIM8) value immediately */
    TIM1->EGR = TIM_EGR_UG; //更新所有寄存器
    
    /* 刹车和死区时间配置 */
    /* Set the Lock level, the Break enable Bit and the Polarity, the OSSR State,
    the OSSI State, the dead time value and the Automatic Output Enable Bit */
#if 0
    TIM1->BDTR &= ~TIM_BDTR_AOE; //禁止自动输出，MOE只能软件置1  
    TIM1->BDTR &= ~TIM_BDTR_BKP; //刹车输入极性-低电平
    TIM1->BDTR &= ~TIM_BDTR_BKE; //刹车输入禁止
    TIM1->BDTR |= TIM_BDTR_OSSR; //当MOE=1且通道为互补输出时，在定时器不工作时，一旦设置CCxE/CCxNE=1，OC/OCN使能并输出无效电平
    TIM1->BDTR &= ~TIM_BDTR_OSSI; //当MOE=0且通道为输出时，在定时器不工作时，禁止OC/OCN输出
    TIM1->BDTR &= ~TIM_BDTR_LOCK; //锁定关闭，寄存器无写保护
    TIM1->BDTR &= ~TIM_BDTR_DTG; //插入互补输出之间的死区时间0 * tDTS
#else
    TIM1->BDTR = 0;
    TIM1->BDTR |= TIM_BDTR_OSSR;
#endif

    /* 定时器比较输出配置 */
    /* Disable the Channel 1: Reset the CC1E and CC1NE Bit */
    TIM1->CCER &= ~(TIM_CCER_CC1E | TIM_CCER_CC1NE);    
    /* Select the Output Compare Mode */
    TIM1->CCMR1 &= ~TIM_CCMR1_OC1M_0;
    TIM1->CCMR1 |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1; //PWM mode 1
    /* Set the Output Compare Polarity */
    TIM1->CCER |= TIM_CCER_CC1P | TIM_CCER_CC1NP; //OC1和OC1N低电平有效
    /* Set the Output Idle state */
    TIM1->CR2 &= ~(TIM_CR2_OIS1N | TIM_CR2_OIS1); //MOE=0时，死区后OC1N=0，OC1=0
    /* Set the Capture Compare Register value */
    TIM1->CCR1 = PWM_Duty; //

#if 0
    /* Enable the tim1 */   
    TIM1->CR1 |= TIM_CR1_CEN;
    /* Enable the main output */
    TIM1->BDTR |= TIM_BDTR_MOE;
    /* Enable the Capture compare channel */
    TIM1->CCER |= TIM_CCER_CC1E; //enable OC1
    /* Disable the complementary PWM output  */
    TIM1->CCER &= ~TIM_CCER_CC1NE; //disable OC1N
#endif 
    SetMotorStop();
    BDCMOTOR_Dir = BDCMOTOR_FORWARD;
}

/**
  * 函数功能: 设置电机速度
  * 输入函数: Duty,输出脉冲占空比
  * 返 回 值: 无
  * 说    明: 无
  */
void SetMotorSpeed(int16_t Duty)
{
    TIM1->CCR1 = Duty;
}

/**
  * 函数功能: 设置电机转动方向
  * 输入函数: Dir,电机转动方向
  * 返 回 值: 无
  * 说    明: 无
  */
void SetMotorDir(int16_t Dir)
{
    if(Dir)
    {
        /* Enable the Capture compare channel */
        TIM1->CCER |= TIM_CCER_CC1E; //enable OC1
        /* Disable the complementary PWM output  */
        TIM1->CCER &= ~TIM_CCER_CC1NE; //disable OC1N
    }
    else
    {
        /* Disable the Capture compare channel */
        TIM1->CCER &= ~TIM_CCER_CC1E; //disable OC1
        /* Enable the complementary PWM output  */
        TIM1->CCER |= TIM_CCER_CC1NE; //enable OC1N
    }
    /* Enable the main output */
    TIM1->BDTR |= TIM_BDTR_MOE;
    /* TIM1 enable counter */
    TIM1->CR1 |= TIM_CR1_CEN;
    
    BDCMOTOR_state = BDCMOTOR_RUN;
}


void SetMotorStop(void)
{
    /* Disable the Capture compare channel */
    /* Disable the complementary PWM output  */
    TIM1->CCER &= ~(TIM_CCER_CC1E | TIM_CCER_CC1NE); //disable OC1, disable OC1N
    /* Disable the Main Output */
    while((TIM1->CCER & (TIM_CCER_CC1E | TIM_CCER_CC1NE)) !=0);
    TIM1->BDTR &= ~TIM_BDTR_MOE;
    /* Disable the Peripheral */
    TIM1->CR1 &= ~TIM_CR1_CEN;
    
    BDCMOTOR_state = BDCMOTOR_IDLE;
}


