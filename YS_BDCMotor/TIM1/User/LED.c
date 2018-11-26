#include "LED.h"

void LED_init(void)
{
	//LED2-PA5
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;  //打开Port A时钟
	GPIOA->MODER &= (~GPIO_MODER_MODER5);
	GPIOA->MODER |=GPIO_MODER_MODER5_0; //PA5输出
	GPIOA->OTYPER &= (~GPIO_OTYPER_OT_5); //PA5推挽输出
	//GPIOA->PUPDR &= (~GPIO_PUPDR_PUPDR5);
	//GPIOA->PUPDR |= GPIO_PUPDR_PUPDR5_0; //PA5 PULL-UP    
}


void LED_ON(void)
{
    GPIOA->ODR |= GPIO_ODR_5;
}

void LED_OFF(void)
{
    GPIOA->ODR &= ~GPIO_ODR_5;
}

void LED_Toggle(void)
{
    GPIOA->ODR ^= GPIO_ODR_5;
}
