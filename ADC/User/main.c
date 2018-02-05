#include "main.h"
#include "adc.h"

void delay(__IO uint32_t delay_cnt)
{
	while(delay_cnt--);
}

uint16_t vr;

int main(void)
{
	//LED2-PA5
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;  //打开Port A时钟
	GPIOA->MODER &= (~GPIO_MODER_MODER5);
	GPIOA->MODER |=GPIO_MODER_MODER5_0; //PA5输出
	GPIOA->OTYPER &= (~GPIO_OTYPER_OT_5); //PA5推挽输出
	//GPIOA->PUPDR &= (~GPIO_PUPDR_PUPDR5);
	//GPIOA->PUPDR |= GPIO_PUPDR_PUPDR5_0; //PA5 PULL-UP
	
	GPIOA->MODER |=GPIO_MODER_MODER0;//PA0模拟输入
	
	//USER_BUTTON-PC13
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN; //打开PORT C时钟
	GPIOC->MODER &= (~GPIO_MODER_MODER13); //PC13设为输入

	ADC_Clock_selection();
	ADC_Calibration();
	ADC_configures();
	ADC_enable();
	ADC_startConversion();

	while(1)
	{
		//vr=ADC_readDR();

		//GPIOA->ODR |= GPIO_ODR_5; //PA5=1
		SET_LED2;
		CLR_SET_LED2_BIT;

		delay(100000+160*vr);
		
		//GPIOA->ODR &= ~GPIO_ODR_5; //PA5=0
		RESET_LED2;
		CLR_RESET_LED2_BIT;

		delay(100000+160*vr);
	}
}

void ADC1_COMP_IRQHandler(void)
{
	vr=ADC_readDR();
}
