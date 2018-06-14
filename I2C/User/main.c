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

int main(void)
{
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
	RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;  //打开I2C1时钟
	RCC->CFGR3 |= RCC_CFGR3_I2C1SW_SYSCLK;//I2C1时钟源选择SYSCLK
	GPIOB->MODER |= GPIO_MODER_MODER7_1 | GPIO_MODER_MODER6_1;//PB6 PB7复用功能
	GPIOB->AFR[0] |= (1<<28) | (1<<24);//PB6-AF1 PB7-AF1
	GPIOB->OSPEEDR |= GPIO_OSPEEDR_OSPEEDR6 | GPIO_OSPEEDR_OSPEEDR7;//PB6 PB7高速
	GPIOB->OTYPER |= GPIO_OTYPER_OT_6 | GPIO_OTYPER_OT_7;//PB6 PB7开漏输出
	GPIOB->PUPDR |= GPIO_PUPDR_PUPDR6_0 | GPIO_PUPDR_PUPDR7_0;//PB6 PB7上拉
   
   //复位I2C1
   RCC->APB1RSTR |= RCC_APB1RSTR_I2C1RST;
   RCC->APB1RSTR &= ~RCC_APB1RSTR_I2C1RST;
   
	
	/* Disable the selected I2C peripheral */
	I2C1->CR1 &= ~I2C_CR1_PE;//先关闭I2C1
	
	/*---------------------------- I2Cx TIMINGR Configuration ------------------*/
  /* Configure I2Cx: Frequency range */
	I2C1->TIMINGR = I2C_TIMING;
	
  /*---------------------------- I2C1 OAR1 Configuration ---------------------*/
  /* Disable Own Address1 before set the Own Address1 configuration */
  I2C1->OAR1 &= ~I2C_OAR1_OA1EN;
	/* Configure I2C1: Own Address1 and ack own address1 mode */
	/* I2C_ADDRESSINGMODE_10BIT */
	I2C1->OAR1 = I2C_OAR1_OA1EN | I2C_OAR1_OA1MODE | I2C_ADDRESS;
	/* I2C_ADDRESSINGMODE_7BIT */
	//I2C1->OAR1 = I2C_OAR1_OA1EN | I2C_ADDRESS&(0x0fe);
	
	/*---------------------------- I2C1 CR2 Configuration ----------------------*/
   /* Configure I2C1: Addressing Master mode */
	/* I2C_ADDRESSINGMODE_10BIT */
	//I2C1->CR2 = I2C_CR2_ADD10;
   /* Enable the AUTOEND by default, and enable NACK (should be disable only during Slave process */
   I2C1->CR2 |= (I2C_CR2_AUTOEND | I2C_CR2_NACK);
	
	/*---------------------------- I2C1 OAR2 Configuration ---------------------*/
   /* Disable Own Address2 before set the Own Address2 configuration */
	I2C1->OAR2 &= ~I2C_OAR2_OA2EN;
   /* Configure I2C1: Dual mode and Own Address2 */
   //I2C1->OAR2 = (I2C_OAR2_OA2EN | I2C_ADDRESS_2 | (I2C_ADDRESS_2_MSK << 8));	
	
  /*---------------------------- I2C1 CR1 Configuration ----------------------*/
  /* Configure I2C1: Generalcall and NoStretch mode */
  I2C1->CR1 &= ~(I2C_CR1_GCEN | I2C_CR1_NOSTRETCH);
	/* Enable the Analog I2C Filter */
	/* Reset I2C1 ANOFF bit */
	I2C1->CR1 &= ~(I2C_CR1_ANFOFF);	
		
  /* Enable the selected I2C peripheral */
  I2C1->CR1 |= I2C_CR1_PE;	

	/*##-1- Start the transmission process #####################################*/  
	/* Timeout is set to 10S */
	/* Send Slave Address */
   I2C1->CR2 = I2C_SLAVE_ADDRESS | I2C_NBYTES<<16 | I2C_CR2_AUTOEND | I2C_CR2_START;
   
	
	while(1)
	{
			GPIOA->ODR ^= GPIO_ODR_5;
			delay(1000);
			//SET_LED2;
			//CLR_SET_LED2_BIT;

			//RESET_LED2;
			//CLR_RESET_LED2_BIT;
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
