#include "main.h"
#include <stdio.h>
#include <string.h>

__IO uint32_t uwTick;
uint8_t rx[10],tx[11]="Ready:GO\r\n",led_light,tx_size,rx_size;

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

void USARTl_init(void)
{
   /******************************************************************
   Asynchronous mode(UART),
   BaudRate=9600,
   1 start bit + 8 bit + 1 stop bit,
   Parity disable,
   TXEIE enable,
   RXNEIE enable,
   EIE enable(Frame error, noise error, overrun error),
   *******************************************************************/
	//USART_TX-PA9; USART_RX-PA10
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;  //打开Port A时钟
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;  //打开USART1时钟
	RCC->CFGR3 |= RCC_CFGR3_USART1SW_SYSCLK;//USART1时钟源选择SYSCLK
	GPIOA->MODER |= GPIO_MODER_MODER10_1 | GPIO_MODER_MODER9_1;//PA9 PA10复用功能
	GPIOA->AFR[1] |= (1<<8) | (1<<4);//PA9-AF1 PA10-AF1
	GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEEDR10 | GPIO_OSPEEDR_OSPEEDR9;//PA9 PA10高速
   GPIOA->OTYPER &= ~GPIO_OTYPER_OT_9;//PA9推挽输出
	GPIOA->OTYPER &= ~GPIO_OTYPER_OT_10;//PA10推挽输出
	GPIOA->PUPDR |= GPIO_PUPDR_PUPDR9_0 | GPIO_PUPDR_PUPDR10_0;//PA9上拉 PA10上拉
   
   //复位USART1
   RCC->APB2RSTR |= RCC_APB2RSTR_USART1RST;
   RCC->APB2RSTR &= ~RCC_APB2RSTR_USART1RST;   
   
   
   //Oversampling by 16, 9600 baudrate
   USART1->BRR = 80000 / 96;// 8000000/9600;
   //Enable DMA in reception and transmission
   USART1->CR3 = USART_CR3_DMAT | USART_CR3_DMAR;   
   //8 data bit, 1 start bit, 1 stop bit, no parity, reception and transmission enabled
   USART1->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;
   /* Polling idle frame Transmission */
   while ((USART1->ISR & USART_ISR_TC) != USART_ISR_TC)
   {
       /* add time out here for a robust application */
   }
   USART1->ICR |= USART_ICR_TCCF; /* Clear TC flag */
   USART1->CR1 |= USART_CR1_TCIE; /* Enable TC interrupt */
#if 0
   while ((USART1->ISR & USART_ISR_IDLE) != USART_ISR_IDLE)
   {
       /* add time out here for a robust application */
   }
   USART1->ICR |= USART_ICR_IDLECF; /* Clear TC flag */
   USART1->CR1 |= USART_CR1_IDLEIE; /* Enable TC interrupt */	 
	 
	/* Configure NVIC for USART1 Interrupt */
	//set USART1 Interrupt to the lowest priority
	NVIC_SetPriority(USART1_IRQn, 0);
	//Enable Interrupt on USART1
	NVIC_EnableIRQ(USART1_IRQn);   	 
#endif
}

void DMA_init(void)
{
	/* (1) Enable the peripheral clock on DMA */
	/* (2) Enable DMA transfer on USART1 */
	/* (3) Configure the peripheral data register address */
	/* (4) Configure the memory address */
	/* (5) Configure the number of DMA tranfer to be performs on channel 2 and 3 */
	/* (6) Configure increment, dirction, size and interrupts */
	/* (7) Enable DMA Channel 2 and 3 */
	RCC->AHBENR |= RCC_AHBENR_DMAEN; /* (1) */
	//USART1->CR3 = USART_CR3_DMAT | USART_CR3_DMAR; /* (2) */
	DMA1_Channel2->CPAR = (uint32_t) (&(USART1->TDR)); /* (3) */
   DMA1_Channel3->CPAR = (uint32_t) (&(USART1->RDR)); /* (3) */
	DMA1_Channel2->CMAR = (uint32_t)(tx); /* (4) */
   DMA1_Channel3->CMAR = (uint32_t)(rx); /* (4) */
	DMA1_Channel2->CNDTR = tx_size;//10;//从10到0，11个数据 /* (5) */
  DMA1_Channel3->CNDTR = rx_size;//9;//从9到0，10个数据 /* (5) */
	DMA1_Channel2->CCR |= DMA_CCR_MINC | DMA_CCR_DIR | DMA_CCR_TEIE | DMA_CCR_TCIE | DMA_CCR_CIRC; /* (6) */
	DMA1_Channel3->CCR |= DMA_CCR_MINC | DMA_CCR_TEIE | DMA_CCR_TCIE | DMA_CCR_CIRC ; /* (6) */ 
	DMA1_Channel2->CCR |= DMA_CCR_EN; /* (7) */
  //DMA1_Channel3->CCR |= DMA_CCR_EN; /* (7) */
	/* Configure NVIC for DMA */
	/* (1) Enable Interrupt on DMA Channel 2 and 3 */
	/* (2) Set priority for DMA Channel 2 and 3 */
	NVIC_EnableIRQ(DMA1_Channel2_3_IRQn); /* (1) */
	NVIC_SetPriority(DMA1_Channel2_3_IRQn,0); /* (2) */  
}

int main(void)
{
	uint8_t i;
	/*Configure the SysTick to have interrupt in 1ms time basis*/
	SysTick_Config(8000);//使用HSI=8MHz作为系统时钟
	
	//LED2-PA5
   LED_init();
	
	//strlen() 函数计算的是字符串的实际长度，遇到第一个'\0'结束，不包括结束字符"\0"。
	tx_size=strlen(tx);//-1;
	if(strlen(rx)>0)rx_size=strlen(rx);//-1;
	else rx_size=9;//0;

	//USART_TX-PA9; USART_RX-PA10
	USARTl_init();//USARTl外设初始化：gpio clock等设置
  DMA_init();

	while(1)
	{
			//GPIOA->ODR ^= GPIO_ODR_5;
      if(strncmp(tx,"LED:ON",sizeof("LED:ON")-1)==0)led_light=1;
      if(strncmp(tx,"LED:OFF",sizeof("LED:OFF")-1)==0)led_light=0;		
		
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

void DMA1_Channel2_3_IRQHandler(void)
{
	uint8_t i;
   /******DMA1_Channel2*****/
   //transfer error
   if(DMA1->ISR & DMA_ISR_TEIF2)
   {
      DMA1->IFCR |= DMA_IFCR_CTEIF2;
   }
   //transfer complete
   if(DMA1->ISR & DMA_ISR_TCIF2)
   {
      DMA1->IFCR |= DMA_IFCR_CTCIF2;
		  DMA1_Channel2->CCR &= ~DMA_CCR_EN;//
		 
		  DMA1_Channel3->CCR |= DMA_CCR_EN; 
   }   
      
   /******DMA1_Channel3*****/
   //transfer error
   if(DMA1->ISR & DMA_ISR_TEIF3)
   {
      DMA1->IFCR |= DMA_IFCR_CTEIF3;
   }
   //transfer complete
   if(DMA1->ISR & DMA_ISR_TCIF3)
   {
		  DMA1->IFCR |= DMA_IFCR_CTCIF3;
		  DMA1_Channel3->CCR &= ~DMA_CCR_EN; 
		 
      strcpy(tx,rx);		
			//strcpy()把rx所指的由NULL结束的字符串复制到tx所指的数组中,返回指向tx字符串的起始地址。
			//所以rx[9]必须为0		 
		  //for(i=0;i<10;i++)rx[i]=0;
     
			tx_size=strlen(tx);//-1;
		  DMA1_Channel2->CNDTR = tx_size;
		  DMA1_Channel2->CCR |= DMA_CCR_EN;
   }    
}
/*
void USART1_IRQHandler(void)
{
   //Frame error
   if(USART1->ISR & USART_ISR_IDLE)
   {
      USART1->ICR |= USART_ICR_IDLECF;
		  rx_size=strlen(rx)-1;
		  DMA1_Channel3->CNDTR = rx_size;
		  DMA1_Channel3->CCR |= DMA_CCR_EN; 
   }   
 }
*/