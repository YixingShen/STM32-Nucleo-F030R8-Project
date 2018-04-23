#include "stm32f0xx.h"

void Disable_Write_protection(void)
{
   /* Disable the write protection for RTC registers */
   RTC->WPR = 0xCA;
   RTC->WPR = 0x53;    
}

void Enable_Write_protection(void)
{
   /* Enable the write protection for RTC registers */
   RTC->WPR = 0xFF;   
}

void Set_Init_mode(void)
{
   /* Set the Initialization mode */
   RTC->ISR |= RTC_ISR_INIT;
   /* Wait till RTC is in INIT state */
   while((RTC->ISR & RTC_ISR_INITF)==0)
   {}
}

void Exit_Init_mode(void)
{
   /* Exit Initialization mode */
   RTC->ISR &= ~RTC_ISR_INIT;
}

void Wait_registers_Synchronization(void)
{
   /* If  RTC_CR_BYPSHAD bit = 0, wait for synchro else this check is not needed */
   if ((RTC->CR & RTC_CR_BYPSHAD) == 0)
   {
      /* Clear RSF flag */
      RTC->ISR &= ~RTC_ISR_RSF;
      /* Wait the registers to be synchronised */
      while((RTC->ISR & RTC_ISR_RSF)==0)
      {}
   }
}

void Calendar_Configuration(void)
{
   /* Check if the Initialization mode is set */
   if ((RTC->ISR & RTC_ISR_INITF) == 0)
   {
      Set_Init_mode();
   }
   
   //set 24 hour format
   RTC->CR &= ~RTC_CR_FMT;
   
   /* Configure the RTC PRER */
   RTC->PRER = 0x7F<<16;//Asynchronous prescaler factor
   RTC->PRER |= 0x137;/* (40KHz / 128) - 1 = 0x137*///Synchronous prescaler factor

   /* Set the date: 2018.4.22.SUNDAY */
   RTC->DR = (uint32_t)(0x18)<<16;//year
   RTC->DR |= (uint32_t)(0x04)<<8;//month
   RTC->DR |= (uint32_t)(0x22);//date
   RTC->DR |= (uint32_t)(0x07)<<13;//weekday

   /* Set the time to 01h 00mn 00s AM */
   RTC->TR = (uint32_t)(0x01)<<16;//hour
   RTC->TR |= (uint32_t)(0x00)<<8;//minute
   RTC->TR |= (uint32_t)(0x04);//second
   
   Exit_Init_mode();
   Wait_registers_Synchronization();
}

void Alarm_A_Configure(void)
{
   /* Set the alarm 01h:00min:04s */ 
   /* Configure the Alarm register */
   RTC->ALRMAR = RTC_ALRMAR_MSK4 | RTC_ALRMAR_WDSEL | ((uint32_t)(0x31)<<24);//date
   RTC->ALRMAR |= RTC_ALRMAR_MSK3 | ((uint32_t)(0x01)<<16);//hour
   RTC->ALRMAR |= RTC_ALRMAR_MSK2 | ((uint32_t)(0x00)<<8);//minute
   RTC->ALRMAR |= RTC_ALRMAR_MSK1 | (uint32_t)(0x04);//second 
   
   //Enable Alarm A interrupt
   RTC->CR |= RTC_CR_ALRAIE;
   
   /* Enable the alarm */
   RTC->CR |= RTC_CR_ALRAE;   
}

void Alarm_A_Interrupt_Configuration(void)
{
   /* RTC Alarm A Interrupt Configuration */
   
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
}

void RTC_Interrupt_Configuration(void)
{
   /* Enable the RTC Alarm Interrupt */
   NVIC_SetPriority(RTC_IRQn, 0);
   NVIC_EnableIRQ(RTC_IRQn);
}

void RTC_Clock_Configuration(void)
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
   
   /* Wait till LSI is ready */
   while((RCC->CSR & RCC_CSR_LSIRDY)==0)
   {}
   
   /* Select the RTC Clock Source */
   RCC->BDCR |= RCC_BDCR_RTCSEL_LSI;
      
   /* Enable the RTC Clock */
   RCC->BDCR |= RCC_BDCR_RTCEN;
}

void RTC_Config(void)
{
   RTC_Clock_Configuration();   
      
   
   Disable_Write_protection();
   Wait_registers_Synchronization();
   /* Calendar Configuration */
   Calendar_Configuration();
   /* Set the alarm 01h:00min:04s */ 
   Alarm_A_Configure();
   Enable_Write_protection();
   
   
   /* Clear Alarm A flag */
   RTC->ISR &= ~RTC_ISR_ALRAF;
   
   /* RTC Alarm A Interrupt Configuration */
   Alarm_A_Interrupt_Configuration();
   
   /* Enable the RTC Alarm Interrupt */
   RTC_Interrupt_Configuration();
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

