#include "main.h"

__IO uint32_t uwTick;
uint8_t flag_step,rx[3],cnt;

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

void I2C_peripheral_init(void)
{
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
   
	/* Configure NVIC for I2C1 Interrupt */
	//set I2C1 Interrupt to the lowest priority
	NVIC_SetPriority(I2C1_IRQn, 0);
	//Enable Interrupt on I2C1
	NVIC_EnableIRQ(I2C1_IRQn);   
   
	/* Disable the selected I2C peripheral */
	I2C1->CR1 &= ~I2C_CR1_PE;//先关闭I2C1
   
   //I2C configured in master mode to transmit code
   /* (1) Timing register value is computed with the AN4235 xls file, fast Mode @400kHz with I2CCLK = 48MHz, rise time = 140ns, fall time = 40ns */
   /* (2) Periph enable */
   I2C1->TIMINGR = (uint32_t)0x00B01A4B; /* (1) */
   I2C1->CR1 = I2C_CR1_PE; /* (2) */

   /* Enable interrupts */
   I2C1->CR1 |= I2C_CR1_ERRIE | I2C_CR1_TCIE | I2C_CR1_STOPIE | I2C_CR1_NACKIE 
              | I2C_CR1_ADDRIE | I2C_CR1_RXIE | I2C_CR1_TXIE;  
}

void I2C_registers_Configuration(uint8_t step)
{
   //I2C configured in master mode to transmit code
   /* (3) Slave address = 0x5A, write transfer, 1 byte to transmit, autoend */
   if(step==0)I2C1->CR2 = I2C_CR2_RELOAD | (1 << 16) | 0xA0; //I2C_CR2_AUTOEND | (1 << 16) | 0xA0; /* (3) */   
   
   //I2C configured in master mode to receive code example
   /* (3) Slave address = 0x5A, read transfer, 1 byte to receive, autoend */
   else if(step==1)I2C2->CR2 = I2C_CR2_AUTOEND | (3<<16) | I2C_CR2_RD_WRN | 0xA1; //I2C_CR2_RELOAD | (3<<16) | I2C_CR2_RD_WRN | 0xA1; /* (3) */
   
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
	I2C_peripheral_init();//I2C外设初始化：gpio clock等设置
   
   /*---------------------------- I2C1 Configuration ----------------------*/
   flag_step=0;
	cnt=0;
   //I2C_registers_Configuration(flag_step);
   
	while(1)
	{
			GPIOA->ODR ^= GPIO_ODR_5;
			delay(1000);
			//SET_LED2;
			//CLR_SET_LED2_BIT;

			//RESET_LED2;
			//CLR_RESET_LED2_BIT;
      
      I2C_registers_Configuration(flag_step);
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


void I2C1_IRQHandler(void)
{
   //ERRIE
   if(I2C1->ISR & I2C_ISR_ALERT)
   {
      I2C1->ICR |= I2C_ICR_ALERTCF;
   }
   if(I2C1->ISR & I2C_ISR_TIMEOUT)
   {
      I2C1->ICR |= I2C_ICR_TIMOUTCF;
   }   
   if(I2C1->ISR & I2C_ISR_PECERR)
   {
      I2C1->ICR |= I2C_ICR_PECCF;
   }   
   if(I2C1->ISR & I2C_ISR_OVR)
   {
      I2C1->ICR |= I2C_ICR_OVRCF;
   }   
   if(I2C1->ISR & I2C_ISR_ARLO)
   {
      I2C1->ICR |= I2C_ICR_ARLOCF;
   }   
   if(I2C1->ISR & I2C_ISR_BERR)
   {
      I2C1->ICR |= I2C_ICR_BERRCF;
   }   
   
   //ADDRIE
   if(I2C1->ISR & I2C_ISR_ADDR)
   {
      I2C1->ICR |= I2C_ICR_ADDRCF;
   }
   
   //Check Tx empty
   if(I2C1->ISR & I2C_ISR_TXE)
   {
      I2C1->TXDR = 0x08;
      I2C1->CR2 |= I2C_CR2_START;
   }
   //传输完成
   if(I2C1->ISR & I2C_ISR_TCR)
   {
      I2C1->CR2 &= ~I2C_CR2_START;
      if(flag_step==0)
      {
         flag_step=1;
         I2C1->CR2 |= (3 << 16);
         //I2C_registers_Configuration(flag_step);
      }
      else 
      {
         //I2C1->CR2 |= I2C_CR2_STOP;
         flag_step=0;
         I2C1->CR2 |= (1 << 16); 
         //I2C_registers_Configuration(flag_step);
      }
      cnt=0;
   }
   
   //I2C master receiver code
   if(I2C1->ISR & I2C_ISR_RXNE)
   {
      rx[cnt]=I2C1->RXDR;
      cnt++;
      //if(cnt==3) I2C1->CR2 |= I2C_CR2_STOP;
   }
   
   //NACKIE
   if(I2C1->ISR & I2C_ISR_NACKF)
   {
      I2C1->ICR |= I2C_ICR_NACKCF;
   }  
   //STOPIE  
   if(I2C1->ISR & I2C_ISR_STOPF)
   {
      I2C1->ICR |= I2C_ICR_STOPCF;
   }     
}

