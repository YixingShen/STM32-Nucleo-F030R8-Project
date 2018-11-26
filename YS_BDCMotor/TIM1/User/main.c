#include "main.h"

__IO uint32_t uwTick;

void delay(__IO uint32_t delay_cnt)//delay_cnt in 1ms
{
   uint32_t tickstart = uwTick;
   uint32_t wait = delay_cnt;

   /* Add a period to guarantee minimum wait */
   if (wait < 0xFFFFFFFF)
   {
      wait++;
   }
  
   while((uwTick-tickstart)<wait){}
}


void TIM_config(void)
{
	//Enable the peripheral clock of Timer 1
	RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
#if 1//使用PLL作为系统时钟
	TIM1->PSC|=3;//Set prescaler to 3, so APBCLK/4 i.e 12MHz
	TIM1->ARR=12000-1;//as timer clock is 12MHz, an event occurs each 1ms
#else //使用HSI作为系统时钟
	TIM1->PSC = 0;//HSI=8MHz
	TIM1->ARR = 8000-1;//as timer clock is 8MHz, an event occurs each 1ms	
#endif	
	TIM1->CR1 &= ~TIM_CR1_DIR;
#if 0	
	TIM1->DIER |= TIM_DIER_UIE;
	/* Configure NVIC for Timer 1 update event */
	NVIC_EnableIRQ(TIM1_BRK_UP_TRG_COM_IRQn); // Enable Interrupt
	NVIC_SetPriority(TIM1_BRK_UP_TRG_COM_IRQn,0); //Set priority for UEV	
#endif	
	TIM1->CR1 |= TIM_CR1_CEN;
}

void MCO_config(void)
{
	//PA8配置成复用功能MCO  
	//RCC->AHBENR |= RCC_AHBENR_GPIOAEN;  //打开Port A时钟
	GPIOA->MODER &= ~GPIO_MODER_MODER8;
	GPIOA->MODER |= GPIO_MODER_MODER8_1;//MODER8[1:0]=10,PA8选择复用功能  
	GPIOA->AFR[1] &=  ~GPIO_AFRH_AFRH0;//PA8选择复用功能AF0，即MCO
	
	//选择MCO上输出的时钟  
	//RCC->CFGR |= (uint32_t)0x80000000U;//not available on stm32f030x8,PLL no divided for MCO
	RCC->CFGR |= RCC_CFGR_MCO_HSI;//RCC_CFGR_MCO_PLL;//RCC_CFGR_MCO_SYSCLK;//
}

int main(void)
{
	PLL_Config();

	/*Configure the SysTick to have interrupt in 1ms time basis*/
	SysTick_Config(SystemCoreClock/1000);
	
    LED_init();
    
	MCO_config();
	
	TIM_config();

	while(1)
	{
        LED_Toggle();
		delay(1000);//1s
	}
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
   uwTick++;
}
