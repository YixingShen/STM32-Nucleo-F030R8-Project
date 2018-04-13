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

void USER_BUTTON_as_EXTI(void)
{
	//USER_BUTTON-PC13
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN; //打开PORT C时钟
	GPIOC->MODER &= (~GPIO_MODER_MODER13); //PC13设为输入

	/* Enable SYSCFG Clock */
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGCOMPEN;
   
	//PC3-EXTI13
	SYSCFG->EXTICR[3] = SYSCFG_EXTICR4_EXTI13_PC;
   
	//不屏蔽EXTI13
	EXTI->IMR |= EXTI_IMR_MR13;
   
	//上升沿触发
	//EXTI->RTSR |= EXTI_RTSR_TR13;
   
	//下降沿触发
	EXTI->FTSR |= EXTI_FTSR_TR13;
   
	/* Configure NVIC for External Interrupt */
	//set EXTI line 4_15 Interrupt to the lowest priority
	NVIC_SetPriority(EXTI4_15_IRQn, 3);//__NVIC_SetPriority(EXTI4_15_IRQn, 0);
	//Enable Interrupt on EXTI4_15
	NVIC_EnableIRQ(EXTI4_15_IRQn);//__NVIC_EnableIRQ(EXTI4_15_IRQn);
}


void WWDG_init(uint16_t prescaler, uint8_t window, uint8_t counter)
{
   /*****************************************
      prescaler=0   div1
                1   div2
                2   div4
                3   div8
   
      window 要小于0x7F，大于0x40，即127和64之间
      counter 要小等于0x7F，大于0x40，即127和64之间，并且要大于window
   *****************************************/
   
   //PCLK=HSI=8MHz
   WWDG->CR = counter;//0x6B;//WWDG counter value=107, WWDG timeout = 1.024ms * 44= 45.056ms
   WWDG->CFR = window;//0x4B;//WWDG window value=75, 计数器的值小于75大于64时才能重载计数器而不会复位
   WWDG->CFR |= prescaler<<7;//WWDG_CFR_WDGTB_0;//WWDG clock counter=8MHz/4096/2=976.5625Hz (1.024ms)
   /*In this case the refresh window is comprised between : 1.024ms * (107-75) = 32.768ms and 1.024ms * 44 = 45.056ms */
   
   WWDG->CR |= WWDG_CR_WDGA;//启动计数器
}

/**
  * @brief  Timeout calculation function.
  *         This function calculates any timeout related to 
  *         WWDG with given prescaler and system clock.
  * @param  timevalue: period in term of WWDG counter cycle.
  * @retval None
  */
static uint32_t TimeoutCalculation(uint8_t prescaler, uint32_t timevalue)
{
  uint32_t timeoutvalue = 0;
  uint32_t pclk1 = 0;
  uint32_t wdgtb = 0;

  /* considering APB divider is still 1, use HCLK value */
  pclk1 = 8000000;

  /* get prescaler */
  wdgtb = (1 << prescaler); /* 2^WDGTB[1:0] */

  /* calculate timeout */
  timeoutvalue = ((4096 * wdgtb * timevalue) / (pclk1 / 1000));

  return timeoutvalue;
}

void Refresh_WWDG(uint8_t counter)
{
   WWDG->CR |= counter;
}

int main(void)
{
   uint32_t dly;
   
   
   /*Configure the SysTick to have interrupt in 1ms time basis*/
   SysTick_Config(8000);//使用HSI=8MHz作为系统时钟
   
	//LED2-PA5
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;  //打开Port A时钟
	GPIOA->MODER &= (~GPIO_MODER_MODER5);
	GPIOA->MODER |=GPIO_MODER_MODER5_0; //PA5输出
	GPIOA->OTYPER &= (~GPIO_OTYPER_OT_5); //PA5推挽输出
	//GPIOA->PUPDR &= (~GPIO_PUPDR_PUPDR5);
	//GPIOA->PUPDR |= GPIO_PUPDR_PUPDR5_0; //PA5 PULL-UP
   
   /*##-1- Check if the system has resumed from WWDG reset ####################*/
   if(RCC->CSR & RCC_CSR_WWDGRSTF)//发生WWDG复位
   {
      /* WWDGRST flag set: Turn LED2 on */
      GPIOA->ODR = GPIO_ODR_5; 
      
      /* Insert 4s delay */
      delay(4000);
      
      /* Prior to clear WWDGRST flag: Turn LED2 off */
      GPIOA->ODR = ~GPIO_ODR_5;     
   }
   
   /* Clear reset flags in any case */
   RCC->CSR |= RCC_CSR_RMVF;
   
   /* Configure User push-button */
   USER_BUTTON_as_EXTI();
   
   /*In this case the refresh window is comprised between : 1.024ms * (107-75) = 32.768ms and 1.024ms * 44 = 45.056ms */
   WWDG_init(1, 75, 107);
   
   /* calculate delay to enter window. Add 1ms to secure round number to upper number  */
   dly = TimeoutCalculation(1, (107-75) + 1) + 1;
   
	while(1)
	{
		//GPIOA->ODR = ~GPIO_ODR_5;
      GPIOA->ODR ^= GPIO_ODR_5;
      
      /* Insert calculated delay */
		delay(dly);
      
      Refresh_WWDG(107);
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


void EXTI4_15_IRQHandler(void)
{
   /* As the following address is invalid (not mapped), a Hardfault exception
   will be generated with an infinite loop and when the WWDG counter falls to 63
   the WWDG reset occurs */   
   *(__IO uint32_t *) 0xA0003000 = 0xFF;
	//if((EXTI->PR&EXTI_PR_PR13) != 0) //PIF13=1 EXTI13中断
	//{
		//CLEAR PIF13
	//	EXTI->PR = EXTI_PR_PR13;
	//}
}
