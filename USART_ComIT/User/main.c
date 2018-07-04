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
   //8 data bit, 1 start bit, 1 stop bit, no parity
   USART1->CR3 = USART_CR3_EIE;
   USART1->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_UE | USART_CR1_RXNEIE | USART_CR1_TCIE;
   while((USART1->ISR & USART_ISR_TC)==0);
   USART1->ICR |= USART_ICR_TCCF;
	 
	/* Configure NVIC for USART1 Interrupt */
	//set USART1 Interrupt to the lowest priority
	NVIC_SetPriority(USART1_IRQn, 0);
	//Enable Interrupt on USART1
	NVIC_EnableIRQ(USART1_IRQn);   
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

	//USART_TX-PA9; USART_RX-PA10
	USARTl_init();//USARTl外设初始化：gpio clock等设置
	tx_cnt=0;
	rx_cnt=0;
	trans_recv=0;//0-发送 1-接收
	flag_rx=0;
	tx_step=0;
   
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


void USART1_IRQHandler(void)
{
   //Frame error
   if(USART1->ISR & USART_ISR_FE)
   {
      USART1->ICR |= USART_ICR_FECF;
   }   
   //noise error
   if(USART1->ISR & USART_ISR_NE)
   {
      USART1->ICR |= USART_ICR_NCF;
   }      
   //overrun error
   if(USART1->ISR & USART_ISR_ORE)
   {
      USART1->ICR |= USART_ICR_ORECF;
   }
   
   
   //Transmission complete
   if(USART1->ISR & USART_ISR_TC)
   {
      USART1->ICR |= USART_ICR_TCCF;
			if(tx_cnt<10)tx_cnt++;
		  if(tx_cnt<tx_size)USART1->TDR = tx[tx_cnt];
			else tx_cnt=0;		 
   }	 
   //Receive data
   if(USART1->ISR & USART_ISR_RXNE)
   {
		  if(rx_cnt>=10)rx_cnt=0;
      rx[rx_cnt] = USART1->RDR;
      if(rx[rx_cnt]=='\n')
      {
         flag_rx=1;
         rx_cnt=0;
      }
      else 
		  {
				if(rx_cnt<10)rx_cnt++;
        else rx_cnt=0;
			}
   }
}

