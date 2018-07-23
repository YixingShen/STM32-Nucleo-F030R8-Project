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
#if 0
   /******************************************************************
   SPI1_NSS:  PA4
   SPI1_SCK:  PB3
   SPI1_MISO: PB4
   SPI1_MOSI: PB5
   *******************************************************************/
   RCC->AHBENR |= RCC_AHBENR_GPIOAEN;  //打开Port A时钟
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
#else
   /******************************************************************
   SPI2_NSS:  PB12
   SPI2_SCK:  PB13
   SPI2_MISO: PB14
   SPI2_MOSI: PB15
   *******************************************************************/
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;  //打开Port B时钟
	RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;  //打开SPI2时钟

   GPIOB->MODER |= GPIO_MODER_MODER12_1 | GPIO_MODER_MODER13_1 | GPIO_MODER_MODER14_1 | GPIO_MODER_MODER15_1;//PB12 PB13 PB14 PB15复用功能
   GPIOB->AFR[1] &= 0x0000FFFF;//PB15-AF0 PB14-AF0 PB13-AF0 PB12-AF0
	GPIOB->OSPEEDR |= GPIO_OSPEEDR_OSPEEDR15 | GPIO_OSPEEDR_OSPEEDR14 | GPIO_OSPEEDR_OSPEEDR13 | GPIO_OSPEEDR_OSPEEDR12;//PB12 PB13 PB14 PB15高速
   GPIOB->OTYPER &= ~(GPIO_OTYPER_OT_15 | GPIO_OTYPER_OT_14 | GPIO_OTYPER_OT_13 | GPIO_OTYPER_OT_12);//PB12 PB13 PB14 PB15推挽输出
	GPIOB->PUPDR |= GPIO_PUPDR_PUPDR15_1 | GPIO_PUPDR_PUPDR14_1 |GPIO_PUPDR_PUPDR13_1 |GPIO_PUPDR_PUPDR12_1;//PB12 PB13 PB14 PB15下拉

   //复位SPI2
   RCC->APB1RSTR |= RCC_APB1RSTR_SPI2RST;
   RCC->APB1RSTR &= ~RCC_APB1RSTR_SPI2RST;   
   
   /******************************************
    空闲状态SCK低电平，第一个SCK边沿采样数据；
    主模式；
    Baud Rate：fpclk/2=4MBits/s，fpclk=fhclk=fsysclk=8MHz；
    MSB在前；
    NSS硬件管理；
    2线全双工；
    不使用CRC；
    ******************************************/
   SPI2->CR1 = SPI_CR1_MSTR;
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
   SPI2->CR2 = SPI_CR2_FRXTH | SPI_CR2_DS_2 | SPI_CR2_DS_1 | SPI_CR2_DS_0 | SPI_CR2_SSOE;
   //          | SPI_CR2_TXEIE | SPI_CR2_RXNEIE | SPI_CR2_ERRIE;
   //使能SPI2
   SPI2->CR1 |= SPI_CR1_SPE;

	/* Configure NVIC for SPI2 Interrupt */
	//set SPI2 Interrupt to the lowest priority
	NVIC_SetPriority(SPI2_IRQn, 0);
	//Enable Interrupt on SPI1
	NVIC_EnableIRQ(SPI2_IRQn);   
#endif
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
#if 0
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
#else
void SPI2_IRQHandler(void)
{
   //int8_t i,error_code=0;
   
   /* SPI in mode Receiver ----------------------------------------------------*/
   if (((SPI2->SR & SPI_SR_OVR) == RESET) &&
      ((SPI2->SR  & SPI_SR_RXNE) != RESET) && ((SPI2->CR2  & SPI_CR2_RXNEIE) != RESET))
   {
      rx[receive_cnt] = SPI2->DR;
      receive_cnt++;
      if(receive_cnt>=receive_size)receive_cnt=0;
      else 
      {
         receive_cnt=0;
         SPI2->CR2 &= ~SPI_CR2_RXNEIE;
      }
      return;
   }

   /* SPI in mode Transmitter -------------------------------------------------*/
   if (((SPI2->SR & SPI_SR_TXE) != RESET) && ((SPI2->CR2 & SPI_CR2_TXEIE) != RESET))
   {
      SPI2->DR = tx[sent_cnt];
      if(sent_cnt<send_size-1)sent_cnt++;
      else 
      {
         sent_cnt=0;
         SPI2->CR2 &= ~SPI_CR2_TXEIE;
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
#endif

