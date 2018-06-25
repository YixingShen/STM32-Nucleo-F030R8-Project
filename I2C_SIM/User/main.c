#include "main.h"
#include "i2cbus.h"

__IO uint32_t uwTick;
uint8_t rx[3];

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

int main(void)
{
	uint8_t i,i1,i2;//m;
	
	/*Configure the SysTick to have interrupt in 1ms time basis*/
	SysTick_Config(8000);//使用HSI=8MHz作为系统时钟
	
	//LED2-PA5
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;  //打开Port A时钟
	GPIOA->MODER &= (~GPIO_MODER_MODER5);
	GPIOA->MODER |=GPIO_MODER_MODER5_0; //PA5输出
	GPIOA->OTYPER &= (~GPIO_OTYPER_OT_5); //PA5推挽输出
	//GPIOA->PUPDR &= (~GPIO_PUPDR_PUPDR5);
	//GPIOA->PUPDR |= GPIO_PUPDR_PUPDR5_0; //PA5 PULL-UP

	
	//I2C1_SCL-PB6; I2C1_SDA-PB7
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;  //打开Port B时钟
	GPIOB->MODER |= GPIO_MODER_MODER6_0;// | GPIO_MODER_MODER7_0;//PB6 PB7输出
	//GPIOB->OSPEEDR |= GPIO_OSPEEDR_OSPEEDR6 | GPIO_OSPEEDR_OSPEEDR7;//PB6 PB7高速
	GPIOB->OTYPER |= GPIO_OTYPER_OT_6;// | GPIO_OTYPER_OT_7;//PB6 PB7开漏输出
	GPIOB->PUPDR |= GPIO_PUPDR_PUPDR6_0;// | GPIO_PUPDR_PUPDR7_0;//PB6 PB7上拉
   

	while(1)
	{
		I2cStart();
		i1=I2cWrite(0xa0);
		if(i1==1)i2=I2cWrite(0x08);
		if(i2==1)
		{
			I2cStart();	
			i=I2cWrite(0xa1);
			if(i==1)
			{
				rx[0]=I2cRead(1);
				rx[1]=I2cRead(1);
				rx[2]=I2cRead(0);
				//m=rx[0]+rx[1];
			}
			else
			{
				rx[0]=0;
				rx[1]=0;
				rx[2]=0;
			}
			I2cStop();
			if(i==1&&(rx[1]!=0||rx[0]!=0))//&&m==rx[2]&&(rx[1]!=0||rx[0]!=0))
			{
				//if(rx[0]&0x01)GPIOA->ODR = GPIO_ODR_5;
				//if(rx[0]&0x02)GPIOA->ODR &= ~GPIO_ODR_5;
				//if(rx[0]&0x04)GPIOA->ODR ^= GPIO_ODR_5;
				LED2_ON;
			}
			else LED2_OFF;
		}
		else I2cStop();		

		delay(20);
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

