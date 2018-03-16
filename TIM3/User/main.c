#include "main.h"


void delay(__IO uint32_t delay_cnt)//delay_cnt in 1ms
{
	while(delay_cnt)
	{
		if(TIM3->SR & TIM_SR_UIF)
		{
			TIM3->SR &= ~TIM_SR_UIF;
			delay_cnt--;
		}
	};
}

void PLL_Config(void)//configuration PLL as system clock
{
	RCC->CFGR |= RCC_CFGR_PLLMUL12;
	RCC->CR |= RCC_CR_PLLON;
	while(RCC->CR&RCC_CR_PLLRDY==0);
	RCC->CFGR |= RCC_CFGR_SW_PLL;
	while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);	
#if 0
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
	RCC->CFGR = RCC->CFGR & (~RCC_CFGR_PLLMUL) | (RCC_CFGR_PLLMUL12); /* (6) PLLSRC=0 HSI/2 selected as PLL input clock; (8MHz/2)*12=48MHz PLLCLK=48MHz MAX*/
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
#endif
}

void TIM_config(void)
{
	//Enable the peripheral clock of Timer 1
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
#if 0//使用PLL作为系统时钟
	TIM3->PSC|=3;//Set prescaler to 3, so APBCLK/4 i.e 12MHz
	TIM3->ARR=12000-1;//as timer clock is 12MHz, an event occurs each 1ms
#else //使用HSI作为系统时钟
	TIM3->PSC = 0;//HSI=8MHz
	TIM3->ARR = 8000-1;//as timer clock is 8MHz, an event occurs each 1ms	
#endif	
	TIM3->CR1 &= ~TIM_CR1_DIR;
#if 1	
	TIM3->DIER |= TIM_DIER_UIE;
	/* Configure NVIC for Timer 1 update event */
	NVIC_EnableIRQ(TIM3_IRQn); // Enable Interrupt
	NVIC_SetPriority(TIM3_IRQn,0); //Set priority for UEV	
#endif	
	TIM3->CR1 |= TIM_CR1_CEN;
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
	RCC->CFGR |= RCC_CFGR_MCO_HSI;//RCC_CFGR_MCO_SYSCLK;//RCC_CFGR_MCO_PLL;//
}
uint16_t time_1ms;
uint8_t time_1s;
int main(void)
{
	//PLL_Config();
	
	//LED2-PA5
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;  //打开Port A时钟
	GPIOA->MODER &= (~GPIO_MODER_MODER5);
	GPIOA->MODER |=GPIO_MODER_MODER5_0; //PA5输出
	GPIOA->OTYPER &= (~GPIO_OTYPER_OT_5); //PA5推挽输出
	//GPIOA->PUPDR &= (~GPIO_PUPDR_PUPDR5);
	//GPIOA->PUPDR |= GPIO_PUPDR_PUPDR5_0; //PA5 PULL-UP
	
	MCO_config();
	
	TIM_config();

	time_1ms=0;
	time_1s=0;
	while(1)
	{
		if(time_1s==2)time_1s=0;
		if(time_1s==0)
		{
			//GPIOA->ODR |= GPIO_ODR_5; //PA5=1
			SET_LED2;
			CLR_SET_LED2_BIT;
		}
		//delay(1000);//1s
		else
		{
			//GPIOA->ODR &= ~GPIO_ODR_5; //PA5=0
			RESET_LED2;
			CLR_RESET_LED2_BIT;
		}
		//delay(1000);//1s
	}
}

#if 1
void TIM3_IRQHandler(void)
{
	if(TIM3->SR & TIM_SR_UIF)
	{
		TIM3->SR &= ~TIM_SR_UIF;
		time_1ms++;
		if(time_1ms>=1000)
		{
			time_1ms=0;
			time_1s++;
		}
	}
}
#endif
