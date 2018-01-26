#include "zero.h"
#include "main.h"

extern void write_flash(uint32_t flash_addr, uint16_t data);
extern uint16_t read_flash(uint32_t flash_addr);
extern uint16_t ReadFlash_before_Erass(void);

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
	
	//USER_BUTTON-PC13
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN; //打开PORT C时钟
	GPIOC->MODER &= (~GPIO_MODER_MODER13); //PC13设为输入
	
	uint8_t i=0;
	uint16_t light,key_press_cnt;
	//首先读出存储的数据
	p_count=ReadFlash_before_Erass();
	light=p_data[0];//read_flash(2);
	while(1)
	{
		if((GPIOC->IDR&GPIO_IDR_13)==0)//按键按下
		{
			if(key_press_cnt>3)
			{
				i++;
				key_press_cnt=0;
				if(light==0)light=1;
				else light=0;
				//write_flash(2,light);
				//write_flash(4,i);
				write_flash(4+2*i,i);
				if(i>=255)i=0;
				write_flash(2,light);
			}
			else key_press_cnt++;
		}			
		else key_press_cnt=0;
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
