#include "BDCMOTOR.h"

uint8_t BDCMOTOR_state,BDCMOTOR_Dir;
__IO int16_t PWM_Duty=BDCMOTOR_DUTY_ZERO; // ռ�ձȣ�PWM_Duty/BDCMOTOR_TIM_PERIOD*100%

void BDCMOTOR_GPIO_Init(void)
{
    /*****************************************
        PIN41:PA8--TIM1_CH1
        PIN34:PB13(NetCN10_30)--TIM1_CH1N
        PIN42:PA9--SHUTDOWN
    *****************************************/
	//PA8
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;  //��Port Aʱ��
	GPIOA->MODER &= (~GPIO_MODER_MODER8);
	GPIOA->MODER |= GPIO_MODER_MODER8_1; //���ù���
	GPIOA->OTYPER &= (~GPIO_OTYPER_OT_8); //�������
    GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEEDR8; //���������ٶ�Ϊ50MHZ  
	GPIOA->AFR[1] &= ~GPIO_AFRH_AFRH0;//���ù���AF2��TIM1_CH1
    GPIOA->AFR[1] |= 0x00000002U;//
    
	//PB13
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;  //��Port Bʱ��
	GPIOB->MODER &= (~GPIO_MODER_MODER13);
	GPIOB->MODER |= GPIO_MODER_MODER13_1; //���ù���
	GPIOB->OTYPER &= (~GPIO_OTYPER_OT_13); //�������
    GPIOB->OSPEEDR |= GPIO_OSPEEDR_OSPEEDR13; //���������ٶ�Ϊ50MHZ  
	GPIOB->AFR[1] &= ~GPIO_AFRH_AFRH5;//���ù���AF2��TIM1_CH1N
    GPIOB->AFR[1] |= 0x00200000U;//

	//PA9
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;  //��Port Aʱ��
	GPIOA->MODER &= (~GPIO_MODER_MODER9);
	GPIOA->MODER |= GPIO_MODER_MODER9_0; //���
	GPIOA->OTYPER &= (~GPIO_OTYPER_OT_9); //�������
    GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEEDR8; //���������ٶ�Ϊ50MHZ  
    GPIOA->ODR &= (~GPIO_ODR_9); //����͵�ƽ
}

void BDCMOTOR_TIMx_Init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN; //ʹ��TIM1ʱ��

    /* Disable slave mode to clock the prescaler directly with the internal clock */
    TIM1->SMCR &= ~TIM_SMCR_SMS; //ʹ���ڲ�ʱ��
    
    /* ��ʱ�������������� */
    TIM1->PSC = BDCMOTOR_TIM_PRESCALER; //CK_CNT = fCK_PSC/(PSC[15:0] + 1)
    TIM1->ARR = BDCMOTOR_TIM_PERIOD;
    TIM1->RCR = BDCMOTOR_TIM_REPETITIONCOUNTER;
    TIM1->CR1 &= ~(TIM_CR1_DIR | TIM_CR1_CMS | TIM_CR1_CKD); //���ϼ��������ض��룬�����������������˲�ʹ�õĲ���ʱ��tDTS�Ͷ�ʱ��ʱ��(CK_INT)����Ƶ
    /* Generate an update event to reload the Prescaler 
       and the repetition counter(only for TIM1 and TIM8) value immediately */
    TIM1->EGR = TIM_EGR_UG; //�������мĴ���
    
    /* ɲ��������ʱ������ */
    /* Set the Lock level, the Break enable Bit and the Polarity, the OSSR State,
    the OSSI State, the dead time value and the Automatic Output Enable Bit */
#if 0
    TIM1->BDTR &= ~TIM_BDTR_AOE; //��ֹ�Զ������MOEֻ�������1  
    TIM1->BDTR &= ~TIM_BDTR_BKP; //ɲ�����뼫��-�͵�ƽ
    TIM1->BDTR &= ~TIM_BDTR_BKE; //ɲ�������ֹ
    TIM1->BDTR |= TIM_BDTR_OSSR; //��MOE=1��ͨ��Ϊ�������ʱ���ڶ�ʱ��������ʱ��һ������CCxE/CCxNE=1��OC/OCNʹ�ܲ������Ч��ƽ
    TIM1->BDTR &= ~TIM_BDTR_OSSI; //��MOE=0��ͨ��Ϊ���ʱ���ڶ�ʱ��������ʱ����ֹOC/OCN���
    TIM1->BDTR &= ~TIM_BDTR_LOCK; //�����رգ��Ĵ�����д����
    TIM1->BDTR &= ~TIM_BDTR_DTG; //���뻥�����֮�������ʱ��0 * tDTS
#else
    TIM1->BDTR = 0;
    TIM1->BDTR |= TIM_BDTR_OSSR;
#endif

    /* ��ʱ���Ƚ�������� */
    /* Disable the Channel 1: Reset the CC1E and CC1NE Bit */
    TIM1->CCER &= ~(TIM_CCER_CC1E | TIM_CCER_CC1NE);    
    /* Select the Output Compare Mode */
    TIM1->CCMR1 &= ~TIM_CCMR1_OC1M_0;
    TIM1->CCMR1 |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1; //PWM mode 1
    /* Set the Output Compare Polarity */
    TIM1->CCER |= TIM_CCER_CC1P | TIM_CCER_CC1NP; //OC1��OC1N�͵�ƽ��Ч
    /* Set the Output Idle state */
    TIM1->CR2 &= ~(TIM_CR2_OIS1N | TIM_CR2_OIS1); //MOE=0ʱ��������OC1N=0��OC1=0
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
  * ��������: ���õ���ٶ�
  * ���뺯��: Duty,�������ռ�ձ�
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
void SetMotorSpeed(int16_t Duty)
{
    TIM1->CCR1 = Duty;
}

/**
  * ��������: ���õ��ת������
  * ���뺯��: Dir,���ת������
  * �� �� ֵ: ��
  * ˵    ��: ��
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


