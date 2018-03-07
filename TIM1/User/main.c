#include "main.h"

uint16_t dly;

void delay(__IO uint32_t delay_cnt)
{
	while(delay_cnt--);
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
	
	//Enable the peripheral clock of Timer 1
	RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
#if 0	
	RCC->CR |= RCC_CR_PLLON;
	RCC->CFGR |= RCC_CFGR_PLLMUL12;
	while(RCC->CR&RCC_CR_PLLRDY==0);
	RCC->CFGR |= RCC_CFGR_SW_PLL;
	while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);
	TIM1->PSC|=3;//Set prescaler to 3, so APBCLK/4 i.e 12MHz
	TIM1->ARR=12000-1;//as timer clock is 12MHz, an event occurs each 1ms
#else
	TIM1->PSC = 0;//
	TIM1->ARR = 8000-1;//as timer clock is 8MHz, an event occurs each 1ms	
#endif	
	TIM1->CR1 &= ~TIM_CR1_DIR;
	TIM1->DIER |= TIM_DIER_UIE;
	TIM1->CR1 |= TIM_CR1_CEN;
	/* Configure NVIC for Timer 1 update event */
	NVIC_EnableIRQ(TIM1_BRK_UP_TRG_COM_IRQn); // Enable Interrupt
	NVIC_SetPriority(TIM1_BRK_UP_TRG_COM_IRQn,0); //Set priority for UEV
	
	dly=2000;
	while(1)
	{
		if(dly>1000)
		{
			//GPIOA->ODR |= GPIO_ODR_5; //PA5=1
			SET_LED2;
			CLR_SET_LED2_BIT;
		}
		//delay(100000);
		else
		{
			//GPIOA->ODR &= ~GPIO_ODR_5; //PA5=0
			RESET_LED2;
			CLR_RESET_LED2_BIT;
		}
		//delay(100000);
	}
}

void TIM1_BRK_UP_TRG_COM_IRQHandler(void)
{
	if(TIM1->SR & TIM_SR_UIF)
	{
		TIM1->SR &= ~TIM_SR_UIF;
		if(dly>0)dly--;
		else dly=2000;
	}
}
