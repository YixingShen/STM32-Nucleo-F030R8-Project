#include "rtc.h"

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
   uint32_t rtc_date,rtc_time;
   
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

   /* Set the date: 2018.4.24.Tuesday*/
   rtc_date = (uint32_t)(0x18)<<16;//year
   rtc_date |= (uint32_t)(0x04)<<8;//month
   rtc_date |= (uint32_t)(0x24);//date
   rtc_date |= (uint32_t)(0x02)<<13;//weekday
   RTC->DR = rtc_date;//

   /* Set the time to 01h 00mn 00s AM */
   rtc_time = (uint32_t)(0x22)<<16;//hour
   rtc_time |= (uint32_t)(0x38)<<8;//minute
   rtc_time |= (uint32_t)(0x00);//second
   RTC->TR = rtc_time;
   
   Exit_Init_mode();
   Wait_registers_Synchronization();
}

void Alarm_A_Configure(void)
{
   uint32_t alarm_time,alarm_sub_second;
   
   /* Set the alarm 01h:00min:04s */ 
   /* Configure the Alarm register */
   alarm_time = ((uint32_t)(0x24)<<24);//date
   alarm_time |= ((uint32_t)(0x22)<<16);//hour
   alarm_time |= ((uint32_t)(0x38)<<8);//minute
   alarm_time |= (uint32_t)(0x04);//second
   RTC->ALRMAR = RTC_ALRMAR_MSK4 
               | RTC_ALRMAR_MSK3 
               | RTC_ALRMAR_MSK2 
               | RTC_ALRMAR_MSK1 
               | alarm_time; 
   
   alarm_sub_second = 39;//Synchronous prescaler factor = 0x137=311, 311/8=39~0.125s
   RTC->ALRMASSR = ((uint32_t)6<<24) | alarm_sub_second;
   
   //Enable Alarm A interrupt
   RTC->CR |= RTC_CR_ALRAIE;
   
   //Alarm A output enabled
   RTC->CR |= RTC_CR_OSEL_0;
   //when ALRAF asserted out low
   RTC->CR |= RTC_CR_POL;
   //RTC_ALARM is a push-pull output
   RTC->TAFCR |= RTC_TAFCR_PC13VALUE;
   
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
   /* Allow access to RTC */
   
   RCC->APB1ENR |= RCC_APB1ENR_PWREN;//使能电源接口时钟
   
   /* Allow access to Backup */
   /* Enable the Backup Domain Access */
   PWR->CR |= PWR_CR_DBP;//使能对RCC_BDCR,RTC寄存器（包括备份寄存器）的访问
   
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

/**
  * @brief  Converts a 2 digit decimal to BCD format.
  * @param  Value: Byte to be converted.
  * @retval Converted byte
  */
#if 0
static uint8_t RTC_ByteToBcd2(uint8_t Value)
{
  uint8_t bcdhigh = 0;
  
  while (Value >= 10)
  {
    bcdhigh++;
    Value -= 10;
  }
  
  return  ((uint8_t)(bcdhigh << 4) | Value);
}
#endif
/**
  * @brief  Convert from 2 digit BCD to Binary.
  * @param  Value: BCD value to be converted.
  * @retval Converted word
  */
static uint8_t RTC_Bcd2ToByte(uint8_t Value)
{
  uint8_t tmp = 0;
  tmp = ((uint8_t)(Value & (uint8_t)0xF0) >> (uint8_t)0x4) * 10;
  return (tmp + (Value & (uint8_t)0x0F));
}

RTC_TimeTypeDef RTC_GetTime(void)
{
   uint32_t tmp32;
   RTC_TimeTypeDef RTC_Time;
   
   tmp32= RTC->TR & 0x007F7F7F;
   
   RTC_Time.RTC_Hours = (uint8_t)((tmp32 & (RTC_TR_HT | RTC_TR_HU)) >> 16);
   RTC_Time.RTC_Minutes = (uint8_t)((tmp32 & (RTC_TR_MNT | RTC_TR_MNU)) >> 8);
   RTC_Time.RTC_Seconds = (uint8_t)(tmp32 & (RTC_TR_ST | RTC_TR_SU));
   RTC_Time.RTC_H12 = (uint8_t)(tmp32 & RTC_TR_PM) >> 22;

   RTC_Time.RTC_Hours = RTC_Bcd2ToByte(RTC_Time.RTC_Hours);
   RTC_Time.RTC_Minutes = RTC_Bcd2ToByte(RTC_Time.RTC_Minutes);
   RTC_Time.RTC_Seconds = RTC_Bcd2ToByte(RTC_Time.RTC_Seconds);
   
   return RTC_Time;
}

RTC_DateTypeDef RTC_GetDate(void)
{
   uint32_t tmp32;
   RTC_DateTypeDef RTC_Date;
   
   tmp32= RTC->DR & 0x00FFFF3F;
   
   RTC_Date.RTC_Year = (uint8_t)((tmp32 & (RTC_DR_YT | RTC_DR_YU)) >> 16);
   RTC_Date.RTC_Month = (uint8_t)((tmp32 & (RTC_DR_MT | RTC_DR_MU)) >> 8);
   RTC_Date.RTC_Date = (uint8_t)(tmp32 & (RTC_DR_DT | RTC_DR_DU));
   RTC_Date.RTC_WeekDay = (uint8_t)(tmp32 & RTC_DR_WDU) >> 13;


   RTC_Date.RTC_Year = RTC_Bcd2ToByte(RTC_Date.RTC_Year);
   RTC_Date.RTC_Month = RTC_Bcd2ToByte(RTC_Date.RTC_Month);
   RTC_Date.RTC_Date = RTC_Bcd2ToByte(RTC_Date.RTC_Date);
   RTC_Date.RTC_WeekDay = RTC_Bcd2ToByte(RTC_Date.RTC_WeekDay);
   
   return RTC_Date;
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

