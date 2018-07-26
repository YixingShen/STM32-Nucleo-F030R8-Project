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

void LED_init(void)
{
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;  //打开Port A时钟
	GPIOA->MODER &= (~GPIO_MODER_MODER5);
	GPIOA->MODER |=GPIO_MODER_MODER5_0; //PA5输出
	GPIOA->OTYPER &= (~GPIO_OTYPER_OT_5); //PA5推挽输出
	//GPIOA->PUPDR &= (~GPIO_PUPDR_PUPDR5);
	//GPIOA->PUPDR |= GPIO_PUPDR_PUPDR5_0; //PA5 PULL-UP   
}

void Button_init(void)
{
	//USER_BUTTON-PC13
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN; //打开PORT C时钟
	GPIOC->MODER &= (~GPIO_MODER_MODER13); //PC13设为输入
}

void SPI_init(void)
{
   /******************************************************************
   SPI2_NSS:  PB12
   SPI2_SCK:  PB13   NetCN10_30
   SPI2_MISO: PB14   NetCN10_28
   SPI2_MOSI: PB15   NetCN10_26
	 PKT	PA1  //input
   RFRST PA0	 
   *******************************************************************/
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;  //打开Port A时钟
	GPIOA->MODER &= ~GPIO_MODER_MODER1;//PA1输入
	GPIOA->MODER |= GPIO_MODER_MODER0_0;//PA0输出	
	GPIOA->OTYPER &= ~(GPIO_OTYPER_OT_1 | GPIO_OTYPER_OT_0);//PA1 PA0推挽输出
	//GPIOA->PUPDR |= GPIO_PUPDR_PUPDR1_1 | GPIO_PUPDR_PUPDR0_1;//PA1 PA0下拉
	GPIOA->PUPDR |= GPIO_PUPDR_PUPDR1_0 | GPIO_PUPDR_PUPDR0_0;//PA1 PA0上拉
	
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;  //打开Port B时钟
	RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;  //打开SPI2时钟

	GPIOB->MODER |= GPIO_MODER_MODER12_0;//PB12输出
   GPIOB->MODER |=  GPIO_MODER_MODER13_1 | GPIO_MODER_MODER14_1 | GPIO_MODER_MODER15_1;//PB13 PB14 PB15复用功能
   GPIOB->AFR[1] &= 0x000FFFFF;//PB15-AF0 PB14-AF0 PB13-AF0
	GPIOB->OSPEEDR |= GPIO_OSPEEDR_OSPEEDR15 | GPIO_OSPEEDR_OSPEEDR14 | GPIO_OSPEEDR_OSPEEDR13 | GPIO_OSPEEDR_OSPEEDR12;//PB12 PB13 PB14 PB15高速
   GPIOB->OTYPER &= ~(GPIO_OTYPER_OT_15 | GPIO_OTYPER_OT_14 | GPIO_OTYPER_OT_13 | GPIO_OTYPER_OT_12);//PB12 PB13 PB14 PB15推挽输出
	GPIOB->PUPDR |= GPIO_PUPDR_PUPDR15_1 | GPIO_PUPDR_PUPDR14_1 |GPIO_PUPDR_PUPDR13_1 |GPIO_PUPDR_PUPDR12_1;//PB12 PB13 PB14 PB15下拉
	//GPIOB->PUPDR |= GPIO_PUPDR_PUPDR15_0 | GPIO_PUPDR_PUPDR14_0 |GPIO_PUPDR_PUPDR13_0 |GPIO_PUPDR_PUPDR12_0;//PB12 PB13 PB14 PB15上拉
	CS_H;//PB12输出高电平
	
   //复位SPI2
   RCC->APB1RSTR |= RCC_APB1RSTR_SPI2RST;
   RCC->APB1RSTR &= ~RCC_APB1RSTR_SPI2RST;   
   
   /******************************************
    空闲状态SCK低电平，第二个SCK边沿采样数据；
    主模式；
    Baud Rate：fpclk/256//fpclk/2=4MBits/s，fpclk=fhclk=fsysclk=8MHz；
    MSB在前；
    NSS软件管理//硬件管理；
    2线全双工；
    不使用CRC；
    ******************************************/
   SPI2->CR1 = SPI_CR1_MSTR | SPI_CR1_CPHA | SPI_CR1_BR | SPI_CR1_SSM | SPI_CR1_SSI;
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
   SPI2->CR2 = SPI_CR2_FRXTH | SPI_CR2_DS_2 | SPI_CR2_DS_1 | SPI_CR2_DS_0 | SPI_CR2_SSOE
             | SPI_CR2_TXEIE | SPI_CR2_RXNEIE | SPI_CR2_ERRIE;
   //使能SPI2
   SPI2->CR1 |= SPI_CR1_SPE;

	/* Configure NVIC for SPI2 Interrupt */
	//set SPI2 Interrupt to the lowest priority
	NVIC_SetPriority(SPI2_IRQn, 0);
	//Enable Interrupt on SPI1
	NVIC_EnableIRQ(SPI2_IRQn);   
}

int main(void)
{
   uint8_t key_press_cnt;
   
	/*Configure the SysTick to have interrupt in 1ms time basis*/
	SysTick_Config(8000);//使用HSI=8MHz作为系统时钟
	
	//LED2-PA5
   LED_init();
   Button_init();
   SPI_init();

   address_code=ADD_INI;
	fun_code=0;
	data_code=0;
	PL1167_Init();
   
	while(1)
	{
		if(KEY_PRESS)//按键按下
		{
			if(key_press_cnt>3)
			{
				key_press_cnt=0;
				flag_RFsend=1;
            if(data_code>8)
				{
					data_code=0;
					if(fun_code==1)fun_code=0;
               else fun_code=1;
				}
				else data_code++;
			}
			else key_press_cnt++;
		}
		else key_press_cnt=0;		
		
		
#ifdef Send_Mode//只发
		if(flag_RFsend)
		{
			flag_RFsend=0;
			Send_DATA(fun_code,data_code);
		}
		else if(0)//(go_sleep==0)//不发送，进入休眠
		{
			//如果休眠，在退出休眠时要初始化，造成LED闪
			go_sleep=1;
			RF_EnterSleep();
		} 
#endif
#ifdef Receive_Mode//只收
		Receive_DATA();   
#endif		
      if(PKT_IS_LOW) LED2_ON;
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
#if 0
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
    LED2_Toggle;
    delay(1000);
  }
}
#endif
uint8_t sent_cnt,receive_cnt;
void SPI2_IRQHandler(void)
{
   //int8_t i,error_code=0;
   uint32_t spixbase = 0x00;
   spixbase = (uint32_t)SPI2; 
   spixbase += 0x0C;
  
   /* SPI in mode Receiver ----------------------------------------------------*/
   if //(((SPI2->SR & SPI_SR_OVR) == RESET) &&
      //((SPI2->SR  & SPI_SR_RXNE) != RESET) && ((SPI2->CR2  & SPI_CR2_RXNEIE) != RESET))
	    ((SPI2->SR  & SPI_SR_RXNE) != RESET)
   {
      rx[receive_cnt] = SPI2->DR;
      receive_cnt++;
      if(receive_cnt>=receive_size)receive_cnt=0;
      else 
      {
         receive_cnt=0;
      }
      return;
   }

   /* SPI in mode Transmitter -------------------------------------------------*/
   if //(((SPI2->SR & SPI_SR_TXE) != RESET) && ((SPI2->CR2 & SPI_CR2_TXEIE) != RESET))
		  ((SPI2->SR & SPI_SR_TXE) != RESET)
   {
      *(__IO uint8_t *) spixbase = tx[sent_cnt];//SPI2->DR = tx[sent_cnt];
      if(sent_cnt<send_size-1)sent_cnt++;
      else 
      {
         sent_cnt=0;
      }
   }
#if 0
   /* SPI in Error Treatment --------------------------------------------------*/
   if (((SPI2->SR & (SPI_SR_MODF | SPI_SR_OVR | SPI_SR_FRE)) != RESET) && ((SPI2->CR2 & SPI_CR2_ERRIE) != RESET))
   {
      /* SPI Overrun error interrupt occurred ----------------------------------*/
      if ((SPI2->SR & SPI_SR_OVR) != RESET)
      {
         error_code=1;
         i = SPI2->DR;
         i = SPI2->SR;
         return;
      }

      /* SPI Mode Fault error interrupt occurred -------------------------------*/
      if ((SPI2->SR & SPI_SR_MODF) != RESET)
      {
         error_code=2;
         i = SPI2->SR;
         SPI2->CR1 &= ~SPI_CR1_SPE;
      }

      /* SPI Frame error interrupt occurred ------------------------------------*/
      if ((SPI2->SR & SPI_SR_FRE) != RESET)
      {
         error_code=3;
         i = SPI2->SR;
      }

      if (error_code != 0)
      {
         if(i!=0)i=0;
         /* Disable all interrupts */
         SPI2->CR2 &= ~(SPI_CR2_RXNEIE | SPI_CR2_TXEIE | SPI_CR2_ERRIE);
         /* Call user error callback */
         Error_Handler();
      }
      return;
   }   
#endif
}


