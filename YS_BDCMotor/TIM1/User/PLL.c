#include "PLL.h"

void PLL_Config(void)//configuration PLL as system clock
{
	RCC->CFGR |= RCC_CFGR_PLLMUL12;
	RCC->CR |= RCC_CR_PLLON;
	while((RCC->CR & RCC_CR_PLLRDY)==0);
	RCC->CFGR |= RCC_CFGR_SW_PLL;
	while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);	
#if 0
	/* (1) Test if PLL is used as System clock */
	/* (2) Select HSI as system clock */
	/* (3) Wait for HSI switched */
	/* (4) Disable the PLL */
	/* (5) Wait until PLLRDY is cleared */
	/* (6) Set the PLL multiplier to 12 */
	/* (7) Enable the PLL */
	/* (8) Wait until PLLRDY is set */
	/* (9) Select PLL as system clock */
	/* (10) Wait until the PLL is switched on */
	if ((RCC->CFGR & RCC_CFGR_SWS) == RCC_CFGR_SWS_PLL) /* (1) */
	{
		RCC->CFGR &= (uint32_t) (~RCC_CFGR_SW); /* (2) */
		while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI) /* (3) */
		{
			/* For robust implementation, add here time-out management */
		}
	}
	RCC->CR &= (uint32_t)(~RCC_CR_PLLON);/* (4) */
	while((RCC->CR & RCC_CR_PLLRDY) != 0) /* (5) */
	{
		/* For robust implementation, add here time-out management */
	}
	RCC->CFGR = RCC->CFGR & (~RCC_CFGR_PLLMUL) | (RCC_CFGR_PLLMUL12); /* (6) PLLSRC=0 HSI/2 selected as PLL input clock; (8MHz/2)*12=48MHz PLLCLK=48MHz MAX*/
	RCC->CR |= RCC_CR_PLLON; /* (7) */
	while((RCC->CR & RCC_CR_PLLRDY) == 0) /* (8) */
	{
		/* For robust implementation, add here time-out management */
	}
	RCC->CFGR |= (uint32_t) (RCC_CFGR_SW_PLL); /* (9) SYSCLK=PLLCLK=48MHz*/
	while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL) /* (10) */
	{
		/* For robust implementation, add here time-out management */
	}
	//HPRE[3:0]=0000 HCLK=SYSCLK ; PPRE[2:0]=000  PCLK=HCLK=48MHz
#endif
    SystemCoreClock=48000000;
}

