#include "main.h"

void LEDInit(void)
{
	//LED2-PA5
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;  //打开Port A时钟
	GPIOA->MODER &= (~GPIO_MODER_MODER5);
	GPIOA->MODER |=GPIO_MODER_MODER5_0; //PA5输出
	GPIOA->OTYPER &= (~GPIO_OTYPER_OT_5); //PA5推挽输出
	//GPIOA->PUPDR &= (~GPIO_PUPDR_PUPDR5);
	//GPIOA->PUPDR |= GPIO_PUPDR_PUPDR5_0; //PA5 PULL-UP
}

void RTC_Config(void)
{
   RCC->APB1ENR |= RCC_APB1ENR_PWREN;
   
   /* Allow access to RTC */
   /* Allow access to Backup */
   /* Enable the Backup Domain Access */
   PWR->CR |= PWR_CR_DBP;   
   
   /* Reset RTC Domain */
   RCC->BDCR |= RCC_BDCR_BDRST;
   RCC->BDCR &= ~RCC_BDCR_BDRST;
   
   /* Enable the LSI */
   RCC->CSR |= RCC_CSR_LSION;
   
   /* Wait till LSE is ready */
   while((RCC->CSR & RCC_CSR_LSIRDY)==0)
   {}
   
   /* Select the RTC Clock Source */
   RCC->BDCR |= RCC_BDCR_RTCSEL_LSI;
      
   /* Enable the RTC Clock */
   RCC->BDCR |= RCC_BDCR_RTCEN;   
#if 0      
   /* Wait for RTC APB registers synchronisation */
   if ((RTC->CR & RTC_CR_BYPSHAD) == 0)//read the calendar through the shadow registers
   {
      /* Disable the write protection for RTC registers */
      RTC->WPR = 0xCA;
      RTC->WPR = 0x53;
      
      /* Clear RSF flag */
      RTC->ISR &= ~RTC_ISR_RSF;
      
      /* Wait the registers to be synchronised */
      while((RTC->ISR & RTC_ISR_RSF)==0)
      {}

      /* Enable the write protection for RTC registers */
      RTC->WPR = 0xFF;
   }
#endif   
   /* Calendar Configuration */
   /* Disable the write protection for RTC registers */
   RTC->WPR = 0xCA;
   RTC->WPR = 0x53; 
   /* Check if the Initialization mode is set */
   if ((RTC->ISR & RTC_ISR_INITF) == 0)
   {
      /* Set the Initialization mode */
      RTC->ISR |= RTC_ISR_INIT;
      
      /* Wait till RTC is in INIT state */
      while((RTC->ISR & RTC_ISR_INITF) == 0)
      {}
   }
   //set 24 hour format
   RTC->CR &= ~RTC_CR_FMT;
   /* Configure the RTC PRER */
   RTC->PRER = 0x7F<<16;//Asynchronous prescaler factor
   RTC->PRER |= 0x137;/* (40KHz / 128) - 1 = 0x137*///Synchronous prescaler factor
   /* Exit Initialization mode */
   RTC->ISR &= ~RTC_ISR_INIT;
   /* Enable the write protection for RTC registers */
   RTC->WPR = 0xFF;
   
   /* Set the alarm 01h:00min:04s */
   /* Disable the write protection for RTC registers */
   RTC->WPR = 0xCA;
   RTC->WPR = 0x53;   
   /* Configure the Alarm register */
   RTC->ALRMAR = RTC_ALRMAR_MSK4 | RTC_ALRMAR_WDSEL | ((uint32_t)(0x31)<<24);//date
   RTC->ALRMAR |= RTC_ALRMAR_MSK3 | ((uint32_t)(0x01)<<16);//hour
   RTC->ALRMAR |= RTC_ALRMAR_MSK2 | ((uint32_t)(0x00)<<8);//minute
   RTC->ALRMAR |= RTC_ALRMAR_MSK1 | (uint32_t)(0x04);//second
   /* Enable the write protection for RTC registers */
   RTC->WPR = 0xFF;   
   
   /* Enable RTC Alarm A Interrupt */
   /* Disable the write protection for RTC registers */
   RTC->WPR = 0xCA;
   RTC->WPR = 0x53;
   //Enable Alarm A interrupt
   RTC->CR |= RTC_CR_ALRAIE;
   /* Enable the write protection for RTC registers */
   RTC->WPR = 0xFF;
   
   /* Enable the alarm */
   /* Disable the write protection for RTC registers */
   RTC->WPR = 0xCA;
   RTC->WPR = 0x53;
   RTC->CR |= RTC_CR_ALRAE;
   /* Enable the write protection for RTC registers */
   RTC->WPR = 0xFF;
   
   /* Set the date: 2018.4.22.SUNDAY */
   /* Disable the write protection for RTC registers */
   RTC->WPR = 0xCA;
   RTC->WPR = 0x53;
   /* Set the Initialization mode */
   RTC->ISR |= RTC_ISR_INIT;
   /* Wait till RTC is in INIT state */
   while((RTC->ISR & RTC_ISR_INITF)==0)
   {}
   RTC->DR = (uint32_t)(0x18)<<16;//year
   RTC->DR |= (uint32_t)(0x04)<<8;//month
   RTC->DR |= (uint32_t)(0x22);//date
   RTC->DR |= (uint32_t)(0x07)<<13;//weekday
   /* Exit Initialization mode */
   RTC->ISR &= ~RTC_ISR_INIT;
   /* If  RTC_CR_BYPSHAD bit = 0, wait for synchro else this check is not needed */
   if ((RTC->CR & RTC_CR_BYPSHAD) == 0)
   {
      /* Clear RSF flag */
      RTC->ISR &= ~RTC_ISR_RSF;
      /* Wait the registers to be synchronised */
      while((RTC->ISR & RTC_ISR_RSF)==0)
      {}
   }
   /* Enable the write protection for RTC registers */
   RTC->WPR = 0xFF;
   
   /* Set the time to 01h 00mn 00s AM */
   /* Disable the write protection for RTC registers */
   RTC->WPR = 0xCA;
   RTC->WPR = 0x53;
   /* Set the Initialization mode */
   RTC->ISR |= RTC_ISR_INIT;
   /* Wait till RTC is in INIT state */
   while((RTC->ISR & RTC_ISR_INITF)==0)
   {}
   RTC->TR = (uint32_t)(0x01)<<16;//hour
   RTC->TR |= (uint32_t)(0x00)<<8;//minute
   RTC->TR |= (uint32_t)(0x04);//second
   /* Exit Initialization mode */
   RTC->ISR &= ~RTC_ISR_INIT;
   /* If  RTC_CR_BYPSHAD bit = 0, wait for synchro else this check is not needed */
   if ((RTC->CR & RTC_CR_BYPSHAD) == 0)
   {
      /* Clear RSF flag */
      RTC->ISR &= ~RTC_ISR_RSF;
      /* Wait the registers to be synchronised */
      while((RTC->ISR & RTC_ISR_RSF)==0)
      {}
   }
   /* Enable the write protection for RTC registers */
   RTC->WPR = 0xFF;

   /* Clear Alarm A flag */
   RTC->ISR & ~RTC_ISR_ALRAF;
   
   /* RTC Alarm A Interrupt Configuration */
   /* EXTI configuration */
   /* Clear EXTI line 17 configuration */
   EXTI->IMR &= ~EXTI_IMR_IM17;
   EXTI->EMR &= ~EXTI_EMR_EM17;
   //NOT mask EXTI line 17
   EXTI->IMR |= EXTI_IMR_IM17;
   /* Clear Rising Falling edge configuration */
   EXTI->RTSR &= ~EXTI_RTSR_RT17;
   EXTI->FTSR &= ~EXTI_FTSR_FT17;
   /* Select Rising edge for EXTI line 17 */
   EXTI->RTSR |= EXTI_RTSR_RT17;
   /* Enable the RTC Alarm Interrupt */
   NVIC_SetPriority(RTC_IRQn, 0);
   NVIC_EnableIRQ(RTC_IRQn);
}

int main(void)
{
   /* Initialize LEDs */
   LEDInit();
   /* Configure RTC */
   RTC_Config();
   
	while(1)
	{

	}
}

void RTC_IRQHandler(void)
{
   if((RTC->ISR & RTC_ISR_ALRAF)!=0)
   {
      GPIOA->ODR ^= GPIO_ODR_5;
      RTC->ISR &= ~RTC_ISR_ALRAF;
      EXTI->PR = EXTI_PR_PIF17;
   }
}
