#include "BDCMOTOR.h"

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
    TIM1->BDTR &= ~TIM_BDTR_AOE; //��ֹ�Զ������MOEֻ�������1  
    TIM1->BDTR &= ~TIM_BDTR_BKP; //ɲ�����뼫��-�͵�ƽ
    TIM1->BDTR &= ~TIM_BDTR_BKE; //ɲ�������ֹ
    TIM1->BDTR |= TIM_BDTR_OSSR; //��MOE=1��ͨ��Ϊ�������ʱ���ڶ�ʱ��������ʱ��һ������CCxE/CCxNE=1��OC/OCNʹ�ܲ������Ч��ƽ
    TIM1->BDTR &= TIM_BDTR_OSSI; //��MOE=0��ͨ��Ϊ���ʱ���ڶ�ʱ��������ʱ����ֹOC/OCN���
    TIM1->BDTR &= ~TIM_BDTR_LOCK; //�����رգ��Ĵ�����д����
    TIM1->BDTR &= ~TIM_BDTR_DTG; //���뻥�����֮�������ʱ��0 * tDTS
    
    /* ��ʱ���Ƚ�������� */
    
    
    
}

void TIM_config(void)
{
	//Enable the peripheral clock of Timer 1
	RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
#if 1//??PLL??????
	TIM1->PSC|=3;//Set prescaler to 3, so APBCLK/4 i.e 12MHz
	TIM1->ARR=12000-1;//as timer clock is 12MHz, an event occurs each 1ms
#else //??HSI??????
	TIM1->PSC = 0;//HSI=8MHz
	TIM1->ARR = 8000-1;//as timer clock is 8MHz, an event occurs each 1ms	
#endif	
	TIM1->CR1 &= ~TIM_CR1_DIR;

	TIM1->CR1 |= TIM_CR1_CEN;
}



void MCO_config(void)
{
	//PA8???????MCO  
	//RCC->AHBENR |= RCC_AHBENR_GPIOAEN;  //??Port A??
	GPIOA->MODER &= ~GPIO_MODER_MODER8;
	GPIOA->MODER |= GPIO_MODER_MODER8_1;//MODER8[1:0]=10,PA8??????  
	GPIOA->AFR[1] &=  ~GPIO_AFRH_AFRH0;//PA8??????AF0,?MCO
	
	//??MCO??????  
	//RCC->CFGR |= (uint32_t)0x80000000U;//not available on stm32f030x8,PLL no divided for MCO
	RCC->CFGR |= RCC_CFGR_MCO_HSI;//RCC_CFGR_MCO_PLL;//RCC_CFGR_MCO_SYSCLK;//
}

