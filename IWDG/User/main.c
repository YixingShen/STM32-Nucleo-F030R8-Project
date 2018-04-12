#include "main.h"


void TIM_config(void)
{
	//Enable the peripheral clock of Timer 1
	RCC->APB2ENR |= RCC_APB2ENR_TIM17EN;
   //使用HSI作为系统时钟
	TIM17->PSC = 0;//HSI=8MHz
	TIM17->ARR = 8000-1;//as timer clock is 8MHz, an event occurs each 1ms	
	//TIM17->CR1 &= ~TIM_CR1_DIR;//TIM6 TIM14 TIM15 TIM16 TIM17计数器只能向上计数，所以没有DIR位
#if 0
	TIM17->DIER |= TIM_DIER_UIE;
	/* Configure NVIC for Timer 15 update event */
	NVIC_EnableIRQ(TIM17_IRQn); // Enable Interrupt
	NVIC_SetPriority(TIM17_IRQn,0); //Set priority for UEV	
#endif	
	TIM17->CR1 |= TIM_CR1_CEN;
}

void IWDG_ini(void)
{
   //RCC->CSR|=RCC_CSR_LSION;
   //while(RCC->CSR & RCC_CSR_LSIRDY ==0);
	/* (1) Activate IWDG (not needed if done in option bytes) */
	/* (2) Enable write access to IWDG registers */
	/* (3) Set prescaler by 8 */
	/* (4) Set reload value to have a rollover each 100ms */
	/* (5) Check if flags are reset */
	/* (6) Refresh counter */
	IWDG->KR = 0x0000CCCC; /* (1) */
	IWDG->KR = 0x00005555; /* (2) */
	IWDG->PR = IWDG_PR_PR_1;//div 16//IWDG_PR_PR_0; /* (3) */
	IWDG->RLR = 2600;//1040ms//500; /* (4) */
	while (IWDG->SR) /* (5) */
	{
		/* add time out here for a robust application */
	}
	IWDG->KR = 0x0000AAAA; /* (6) */    
}

void feed_WDG(void)
{
   IWDG->KR = 0x0000AAAA; 
}

void delay(__IO uint32_t delay_cnt)//delay_cnt in 1ms
{
	while(delay_cnt)
	{
      //if(delay_cnt%50==0)feed_WDG();
		if(TIM17->SR & TIM_SR_UIF)
		{
			TIM17->SR &= ~TIM_SR_UIF;
			delay_cnt--;
		}
	};
}

int main(void)
{
	//LED2-PA5
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;  //打开Port A时钟
	GPIOA->MODER &= (~GPIO_MODER_MODER5);
	GPIOA->MODER |=GPIO_MODER_MODER5_0; //PA5输出
	GPIOA->OTYPER &= (~GPIO_OTYPER_OT_5); //PA5推挽输出
	//GPIOA->PUPDR &= (~GPIO_PUPDR_PUPDR5);
	//GPIOA->PUPDR |= GPIO_PUPDR_PUPDR5_0; //PA5 PULL-UP
   //GPIOA->ODR = GPIO_ODR_5;
   
	TIM_config();
   
   IWDG_ini();

	while(1)
	{
      feed_WDG();
		//GPIOA->ODR = ~GPIO_ODR_5;
      GPIOA->ODR ^= GPIO_ODR_5;
		delay(1000);//1s
	}
}

#if 0
void TIM17_IRQHandler(void)
{
	if(TIM17->SR & TIM_SR_UIF)
	{
		TIM17->SR &= ~TIM_SR_UIF;
		time_1ms++;
		if(time_1ms>=1000)
		{
			time_1ms=0;
			time_1s++;
		}
	}
}
#endif
