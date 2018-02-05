#include "stm32f030x8.h"

void ADC_Calibration(void)
{
	/* (1) Ensure that ADEN = 0 */
	/* (2) Clear ADEN by setting ADDIS*/
	/* (3) Clear DMAEN */
	/* (4) Launch the calibration by setting ADCAL */
	/* (5) Wait until ADCAL=0 */
	if ((ADC1->CR & ADC_CR_ADEN) != 0) /* (1) */
	{
			ADC1->CR |= ADC_CR_ADDIS; /* (2) */
	}
	while ((ADC1->CR & ADC_CR_ADEN) != 0)
	{
			/* For robust implementation, add here time-out management */
	}
	ADC1->CFGR1 &= ~ADC_CFGR1_DMAEN; /* (3) */
	ADC1->CR |= ADC_CR_ADCAL; /* (4) */
	while ((ADC1->CR & ADC_CR_ADCAL) != 0) /* (5) */
	{
			/* For robust implementation, add here time-out management */
	}  	
}

void ADC_enable(void)
{
	/* (1) Ensure that ADRDY = 0 */
	/* (2) Clear ADRDY */
	/* (3) Enable the ADC */
	/* (4) Wait until ADC ready */
	if ((ADC1->ISR & ADC_ISR_ADRDY) != 0) /* (1) */
	{
			ADC1->ISR |= ADC_ISR_ADRDY; /* (2) */
	}
	ADC1->CR |= ADC_CR_ADEN; /* (3) */
	while ((ADC1->ISR & ADC_ISR_ADRDY) == 0) /* (4) */
	{
			/* For robust implementation, add here time-out management */
	}
}

void ADC_disable(void)
{
	/* (1) Stop any ongoing conversion */
	/* (2) Wait until ADSTP is reset by hardware i.e. conversion is stopped */
	/* (3) Disable the ADC */
	/* (4) Wait until the ADC is fully disabled */
	ADC1->CR |= ADC_CR_ADSTP; /* (1) */
	while ((ADC1->CR & ADC_CR_ADSTP) != 0) /* (2) */
	{
			/* For robust implementation, add here time-out management */
	}
	ADC1->CR |= ADC_CR_ADDIS; /* (3) */
	while ((ADC1->CR & ADC_CR_ADEN) != 0) /* (4) */
	{
			/* For robust implementation, add here time-out management */
	}  	
}

void ADC_Clock_selection(void)
{
	/* This code selects the HSI14 as clock source. */
	/* (1) Enable the peripheral clock of the ADC */
	/* (2) Start HSI14 RC oscillator */
	/* (3) Wait HSI14 is ready */
	/* (4) Select HSI14 by writing 00 in CKMODE (reset value) */
	RCC->APB2ENR |= RCC_APB2ENR_ADCEN; /* (1) */
	RCC->CR2 |= RCC_CR2_HSI14ON; /* (2) */
	while ((RCC->CR2 & RCC_CR2_HSI14RDY) == 0) /* (3) */
	{
			/* For robust implementation, add here time-out management */
	}
	ADC1->CFGR2 &= (~ADC_CFGR2_CKMODE); /* (4) */  	
}

void ADC_configures(void)
{
	/* This code example configures the AD conversion in continuous and wait,overrun mode and in upward scan. It also enable the interrupts. */
	/* (1) Select scanning direction, data resolution and alignment */
	/* (2) Select the continuous mode and wait mode, overrun mode */
	/* (3) Select CHSEL0 */
	/* (4) Select a sampling mode of 111 i.e. 239.5 ADC clk to be greater than 17.1us */
	/* (5) Enable interrupts on EOC, EOSEQ and overrrun */
	/* (6) Wake-up the VREFINT (only for VBAT, Temp sensor and VRefInt) */
	ADC1->CFGR1 &= (~(ADC_CFGR1_SCANDIR | ADC_CFGR1_RES | ADC_CFGR1_ALIGN)); /* (1) */
	ADC1->CFGR1 |= ADC_CFGR1_CONT | ADC_CFGR1_WAIT| ADC_CFGR1_OVRMOD; /* (2) */
	ADC1->CHSELR = ADC_CHSELR_CHSEL0; /* (3) */
	ADC1->SMPR |= ADC_SMPR_SMP_0 | ADC_SMPR_SMP_1 | ADC_SMPR_SMP_2; /* (4) */
//	ADC1->IER = ADC_IER_EOCIE | ADC_IER_EOSEQIE | ADC_IER_OVRIE; /* (5) */
	ADC->CCR |= ADC_CCR_VREFEN; /* (6) */
	/* Configure NVIC for ADC */
	/* (7) Enable Interrupt on ADC */
	/* (8) Set priority for ADC */
//	NVIC_EnableIRQ(ADC1_COMP_IRQn); /* (7) */
//	NVIC_SetPriority(ADC1_COMP_IRQn,0); /* (8) */ 	
}

void ADC_startConversion(void)
{
	if(ADC1->ISR & ADC_ISR_ADRDY)//(ADC1->CR & ADC_CR_ADEN!=0 &&ADC1->CR & ADC_CR_ADDIS==0)
	{
		ADC1->CR |= ADC_CR_ADSTART;
	}
}

uint16_t ADC_readDR(void)
{
	uint16_t uint16=0;
	if(ADC1->IER & ADC_IER_EOCIE || ADC1->IER & ADC_IER_EOSEQIE)
	{
		uint16=ADC1->DR;
		if(ADC1->IER & ADC_IER_EOCIE) ADC1->IER |= (~ADC_IER_EOCIE);
		if(ADC1->IER & ADC_IER_EOSEQIE) ADC1->IER |= (~ADC_IER_EOSEQIE);
	}
	return uint16;
}
