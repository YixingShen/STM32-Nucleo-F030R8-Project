#include "main.h"

//uint16_t dly;

void delay(__IO uint32_t delay_cnt)//delay_cnt in 1ms
{
	while(delay_cnt)
	{
		if(TIM1->SR & TIM_SR_UIF)
		{
			TIM1->SR &= ~TIM_SR_UIF;
			delay_cnt--;
		}
	};
}

void PLL_Config(void)//configuration PLL as system clock
{
//	RCC->CR |= RCC_CR_PLLON;
//	RCC->CFGR |= RCC_CFGR_PLLMUL12;
//	while(RCC->CR&RCC_CR_PLLRDY==0);
//	RCC->CFGR |= RCC_CFGR_SW_PLL;
//	while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);	
	/* (1) Test if PLL is used as System clock */
	/* (2) Select HSI as system clock */
	/* (3) Wait for HSI switched */
	/* (4) Disable the PLL */
	/* (5) Wait until PLLRDY is cleared */
	/* (6) Set the PLL multiplier to 12 */
	/* (7) Enable the PLL */
	/* (8) Wait until PLLRDY is set */
	/* (9) Select PLL as system clock */
	/* (10) Wait until the PLL is switched on */
	if ((RCC->CFGR & RCC_CFGR_SWS) == RCC_CFGR_SWS_PLL) /* (1) */
	{
		RCC->CFGR &= (uint32_t) (~RCC_CFGR_SW); /* (2) */
		while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI) /* (3) */
		{
			/* For robust implementation, add here time-out management */
		}
	}
	RCC->CR &= (uint32_t)(~RCC_CR_PLLON);/* (4) */
	while((RCC->CR & RCC_CR_PLLRDY) != 0) /* (5) */
	{
		/* For robust implementation, add here time-out management */
	}
	RCC->CFGR = RCC->CFGR & (~RCC_CFGR_PLLMUL) | (RCC_CFGR_PLLMUL12); /* (6) PLLSRC=0 HSI/2 selected as PLL input clock; (8MHz/2)*12=48MHz PLLCLK=48MHz*/
	RCC->CR |= RCC_CR_PLLON; /* (7) */
	while((RCC->CR & RCC_CR_PLLRDY) == 0) /* (8) */
	{
		/* For robust implementation, add here time-out management */
	}
	RCC->CFGR |= (uint32_t) (RCC_CFGR_SW_PLL); /* (9) SYSCLK=PLLCLK=48MHz*/
	while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL) /* (10) */
	{
		/* For robust implementation, add here time-out management */
	}
	//HPRE[3:0]=0000 HCLK=SYSCLK ; PPRE[2:0]=000  PCLK=HCLK=48MHz
}

void TIM_config(void)
{
	//Enable the peripheral clock of Timer 1
	RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
#if 0	
	TIM1->PSC|=3;//Set prescaler to 3, so APBCLK/4 i.e 12MHz
	TIM1->ARR=12000-1;//as timer clock is 12MHz, an event occurs each 1ms
#else
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

int main(void)
{
	//LED2-PA5
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;  //打开Port A时钟
	GPIOA->MODER &= (~GPIO_MODER_MODER5);
	GPIOA->MODER |=GPIO_MODER_MODER5_0; //PA5输出
	GPIOA->OTYPER &= (~GPIO_OTYPER_OT_5); //PA5推挽输出
	//GPIOA->PUPDR &= (~GPIO_PUPDR_PUPDR5);
	//GPIOA->PUPDR |= GPIO_PUPDR_PUPDR5_0; //PA5 PULL-UP
	
	TIM_config();
	
	//dly=2000;
	while(1)
	{
		//if(dly>1000)
		//{
			//GPIOA->ODR |= GPIO_ODR_5; //PA5=1
			SET_LED2;
			CLR_SET_LED2_BIT;
		//}
		delay(1000);//1s
		//else
		//{
			//GPIOA->ODR &= ~GPIO_ODR_5; //PA5=0
			RESET_LED2;
			CLR_RESET_LED2_BIT;
		//}
		delay(1000);//1s
	}
}

#if 0
void TIM1_BRK_UP_TRG_COM_IRQHandler(void)
{
	if(TIM1->SR & TIM_SR_UIF)
	{
		TIM1->SR &= ~TIM_SR_UIF;
		if(dly>0)dly--;
		else dly=2000;
	}
}
#endif
