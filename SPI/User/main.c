#include "main.h"
#include <stdio.h>
#include <string.h>

__IO uint32_t uwTick;
uint8_t rx[10],rx_cnt,tx[10],tx_cnt,trans_recv,tx_size,flag_rx,led_light,tx_step;

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

void LED_init(void)
{
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;  //打开Port A时钟
	GPIOA->MODER &= (~GPIO_MODER_MODER5);
	GPIOA->MODER |=GPIO_MODER_MODER5_0; //PA5输出
	GPIOA->OTYPER &= (~GPIO_OTYPER_OT_5); //PA5推挽输出
	//GPIOA->PUPDR &= (~GPIO_PUPDR_PUPDR5);
	//GPIOA->PUPDR |= GPIO_PUPDR_PUPDR5_0; //PA5 PULL-UP   
}

void SPI_init(void)
{
   /******************************************************************
   SPI1_NSS:  PA4
   SPI1_SCK:  PB3
   SPI1_MISO: PB4
   SPI1_MOSI: PB5
   *******************************************************************/
   RCC->AHBENR |= RCC_AHBENR_GPIOAEN;  //打开Port A时钟S
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;  //打开Port B时钟
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;  //打开SPI1时钟
	GPIOA->MODER |= GPIO_MODER_MODER4_1;//PA4复用功能
   GPIOB->MODER |= GPIO_MODER_MODER5_1 | GPIO_MODER_MODER4_1 | GPIO_MODER_MODER3_1;//PB3 PB4 PB5复用功能
	GPIOA->AFR[0] &= 0xFFF0FFFF;//PA4-AF0
   GPIOB->AFR[0] &= 0xFF000FFF;//PB5-AF0 PB4-AF0 PB3-AF0 
	GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEEDR4;//PA4高速
   GPIOA->OTYPER &= ~GPIO_OTYPER_OT_4;//PA4推挽输出
	GPIOA->PUPDR |= GPIO_PUPDR_PUPDR4_1;//PA4下拉

	GPIOB->OSPEEDR |= GPIO_OSPEEDR_OSPEEDR5 | GPIO_OSPEEDR_OSPEEDR4 | GPIO_OSPEEDR_OSPEEDR3;//PB3 PB4 PB5高速
   GPIOB->OTYPER &= ~(GPIO_OTYPER_OT_5 | GPIO_OTYPER_OT_4 | GPIO_OTYPER_OT_3);//PB3 PB4 PB5推挽输出
	GPIOB->PUPDR |= GPIO_PUPDR_PUPDR5_1 | GPIO_PUPDR_PUPDR4_1 |GPIO_PUPDR_PUPDR3_1;//PB3 PB4 PB5下拉
   
   //复位SPI1
   RCC->APB2RSTR |= RCC_APB2RSTR_SPI1RST;
   RCC->APB2RSTR &= ~RCC_APB2RSTR_SPI1RST;   
   
   /******************************************
    空闲状态SCK低电平，第一个SCK边沿采样数据；
    主模式；
    Baud Rate：fpclk/2=4MBits/s，fpclk=fhclk=fsysclk=8MHz；
    MSB在前；
    NSS硬件管理；
    2线全双工；
    不使用CRC；
    ******************************************/
   SPI1->CR1 = SPI_CR1_MSTR;
   /******************************************
    不使用TX和RX DMA；
    NSS输出使能；
    不使用NSS脉冲模式；
    使用Motorola模式；
    错误中断使能；
    RXNE中断使能；
    TXE中断使能；
    数据长度：8bit；
    接收阈值：8bit；
    ******************************************/   
   SPI1->CR2 = SPI_CR2_FRXTH | SPI_CR2_DS_2 | SPI_CR2_DS_1 | SPI_CR2_DS_0 
             | SPI_CR2_TXEIE | SPI_CR2_RXNEIE | SPI_CR2_ERRIE | SPI_CR2_SSOE;
   //使能SPI1
   SPI1->CR1 |= SPI_CR1_SPE;

	/* Configure NVIC for SPI1 Interrupt */
	//set SPI1 Interrupt to the lowest priority
	NVIC_SetPriority(SPI1_IRQn, 0);
	//Enable Interrupt on SPI1
	NVIC_EnableIRQ(SPI1_IRQn);   
}

void commu(void)
{
   if(trans_recv==0)//发送
   {
      trans_recv=1;
      
      if(tx_step<2)
      {
         if(tx_step==0)sprintf(tx,"Ready\r\n");

				 tx_size=strlen(tx);//strlen() 函数计算的是字符串的实际长度，遇到第一个'\0'结束，不包括结束字符"\0"。
				 
				 USART1->TDR = tx[0];
			}
			else tx_size=0;
			tx_step=2;
   }
   else//接收
   {
      if(flag_rx==1)
      {
				 tx_step=1;
				 flag_rx=0;
				 trans_recv=0;
         if(strncmp(rx,"LED:ON",sizeof("LED:ON")-1)==0)led_light=1;
         else if(strncmp(rx,"LED:OFF",sizeof("LED:OFF")-1)==0)led_light=0;
         strcpy(tx,rx);
				 //strcpy()把rx所指的由NULL结束的字符串复制到tx所指的数组中,返回指向tx字符串的起始地址。
				 //所以rx[9]必须为0
      }
   }
}

int main(void)
{
	/*Configure the SysTick to have interrupt in 1ms time basis*/
	SysTick_Config(8000);//使用HSI=8MHz作为系统时钟
	
	//LED2-PA5
   LED_init();

	while(1)
	{
			//GPIOA->ODR ^= GPIO_ODR_5;
			commu();
      
			if(led_light==1)GPIOA->ODR |= GPIO_ODR_5;
			else GPIOA->ODR &= ~GPIO_ODR_5;

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

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
static void Error_Handler(void)
{
  while(1)
  {
    /* Toggle LED2 */
    GPIOA->ODR ^= GPIO_ODR_5;
    delay(1000);
  }
}

void SPI1_IRQHandler(void)
{
   int8_t i,error_code=0;
   
   /* SPI in mode Receiver ----------------------------------------------------*/
   if (((SPI1->SR & SPI_SR_OVR) == RESET) &&
      ((SPI1->SR  & SPI_SR_RXNE) != RESET) && ((SPI1->CR2  & SPI_CR2_RXNEIE) != RESET))
   {
      rx[0] = SPI1->DR;
      return;
   }

   /* SPI in mode Transmitter -------------------------------------------------*/
   if (((SPI1->SR & SPI_SR_TXE) != RESET) && ((SPI1->CR2 & SPI_CR2_TXEIE) != RESET))
   {
      SPI1->DR = tx[0];
      return;
   }

   /* SPI in Error Treatment --------------------------------------------------*/
   if (((SPI1->SR & (SPI_SR_MODF | SPI_SR_OVR | SPI_SR_FRE)) != RESET) && ((SPI1->CR2 & SPI_CR2_ERRIE) != RESET))
   {
      /* SPI Overrun error interrupt occurred ----------------------------------*/
      if ((SPI1->SR & SPI_SR_OVR) != RESET)
      {
         error_code=1;
         i = SPI1->DR;
         i = SPI1->SR;
         return;
      }

      /* SPI Mode Fault error interrupt occurred -------------------------------*/
      if ((SPI1->SR & SPI_SR_MODF) != RESET)
      {
         error_code=2;
         i = SPI1->SR;
         SPI1->CR1 &= ~SPI_CR1_SPE;
      }

      /* SPI Frame error interrupt occurred ------------------------------------*/
      if ((SPI1->SR & SPI_SR_FRE) != RESET)
      {
         error_code=3;
         i = SPI1->SR;
      }

      if (error_code != 0)
      {
         /* Disable all interrupts */
         SPI1->CR2 &= ~(SPI_CR2_RXNEIE | SPI_CR2_TXEIE | SPI_CR2_ERRIE);
         /* Call user error callback */
         Error_Handler();
      }
    return;
  }   
}
