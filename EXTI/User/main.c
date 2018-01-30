#include "main.h"


void delay(__IO uint32_t delay_cnt)
{
	while(delay_cnt--);
}

uint16_t light;

int main(void)
{
	//LED2-PA5
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;  //打开Port A时钟
	GPIOA->MODER &= (~GPIO_MODER_MODER5);
	GPIOA->MODER |=GPIO_MODER_MODER5_0; //PA5输出
	GPIOA->OTYPER &= (~GPIO_OTYPER_OT_5); //PA5推挽输出
	//GPIOA->PUPDR &= (~GPIO_PUPDR_PUPDR5);
	//GPIOA->PUPDR |= GPIO_PUPDR_PUPDR5_0; //PA5 PULL-UP
	
	//USER_BUTTON-PC13
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN; //打开PORT C时钟
	GPIOC->MODER &= (~GPIO_MODER_MODER13); //PC13设为输入
	/* Enable SYSCFG Clock */
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGCOMPEN;
	//PC3-EXTI13
	SYSCFG->EXTICR[3] = SYSCFG_EXTICR4_EXTI13_PC;
	//不屏蔽EXTI13
	EXTI->IMR |= EXTI_IMR_MR13;
	//上升沿触发
	EXTI->RTSR |= EXTI_RTSR_TR13;
	//下降沿触发
	EXTI->FTSR |= EXTI_FTSR_TR13;
	/* Configure NVIC for External Interrupt */
	//set EXTI line 4_15 Interrupt to the lowest priority
	__NVIC_SetPriority(EXTI4_15_IRQn, 0);
	//Enable Interrupt on EXTI4_15
	__NVIC_EnableIRQ(EXTI4_15_IRQn);

	light=0;
	while(1)
	{
		if(light==1)//0xffff)//==1)
		{
			//GPIOA->ODR |= GPIO_ODR_5; //PA5=1
			SET_LED2;
			CLR_SET_LED2_BIT;
		}
		else
		{
			//GPIOA->ODR &= ~GPIO_ODR_5; //PA5=0
			RESET_LED2;
			CLR_RESET_LED2_BIT;
		}
		delay(100000);
	}
}



void EXTI4_15_IRQHandler(void)
{
	if((EXTI->PR&EXTI_PR_PR13) != 0) //PIF13=1
	{
		//CLEAR PIF13
		EXTI->PR = EXTI_PR_PR13;
		if(light==0)light=1;
		else light=0;
	}
}

