#include "main.h"

void PLL_Config(void)//configuration PLL as system clock
{
	RCC->CFGR |= RCC_CFGR_PLLMUL12;
	RCC->CR |= RCC_CR_PLLON;
	while(RCC->CR&RCC_CR_PLLRDY==0);
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
}

void TIM_config(void)//配置为PWM输入模式
{
	//PA2配置成复用功能TIM15_CH1
	//RCC->AHBENR |= RCC_AHBENR_GPIOAEN;  //打开Port A时钟
	GPIOA->MODER &= ~GPIO_MODER_MODER2;
	GPIOA->MODER |= GPIO_MODER_MODER2_1;//MODER2[1:0]=10,PA2选择复用功能  
	GPIOA->AFR[0] &=  ~GPIO_AFRL_AFRL2;//PA2选择复用功能AF0，即TIM15_CH1
   
	//Enable the peripheral clock of Timer 1
	RCC->APB2ENR |= RCC_APB2ENR_TIM15EN;
#if 0//使用PLL作为系统时钟
	TIM15->PSC|=3;//Set prescaler to 3, so APBCLK/4 i.e 12MHz
	TIM15->ARR=180000-1;//as timer clock is 12MHz, an event occurs each 15ms
#else //使用HSI作为系统时钟
	TIM15->PSC = 0;//HSI=8MHz
	TIM15->ARR = 120000-1;//as timer clock is 8MHz, an event occurs each 15ms	
#endif	
	//TIM15->CR1 &= ~TIM_CR1_DIR;//TIM6 TIM14 TIM15计数器只能向上计数，所以没有DIR位
   
   TIM15->CCMR1 |= TIM_CCMR1_CC1S_0 | TIM_CCMR1_CC2S_1; //CC1S = 01 选择CH1作为TIM15_CCR1源, CC2S = 10 选择CH1作为TIM15_CCR2源
   TIM15->CCER |= TIM_CCER_CC1P | TIM_CCER_CC1E | TIM_CCER_CC2E;//CC1NP/CC1P=01，TI1FP1下降沿触发；CC2NP/CC2P=00，TI1FP2上升沿触发
   TIM15->SMCR |= TIM_SMCR_TS_2 | TIM_SMCR_TS_0 | TIM_SMCR_SMS_2;//TS=101，选择TI1FP1作为从模式控制器触发源；SMS=100，从模式控制器选择复位模式
   
	TIM15->DIER |= TIM_DIER_UIE | TIM_DIER_CC1IE | TIM_DIER_CC2IE; //使能IC1和IC2捕获中断，更新中断
	/* Configure NVIC for Timer 15 update event */
	NVIC_EnableIRQ(TIM15_IRQn); // Enable Interrupt
	NVIC_SetPriority(TIM15_IRQn,0); //Set priority for UEV	

	TIM15->CR1 |= TIM_CR1_CEN;
}

void MCO_config(void)
{
	//PA8配置成复用功能MCO  
	//RCC->AHBENR |= RCC_AHBENR_GPIOAEN;  //打开Port A时钟
	GPIOA->MODER &= ~GPIO_MODER_MODER8;
	GPIOA->MODER |= GPIO_MODER_MODER8_1;//MODER8[1:0]=10,PA8选择复用功能  
	GPIOA->AFR[1] &=  ~GPIO_AFRH_AFRH0;//PA8选择复用功能AF0，即MCO
	
	//选择MCO上输出的时钟  
	//RCC->CFGR |= (uint32_t)0x80000000U;//not available on stm32f030x8,PLL no divided for MCO
	RCC->CFGR |= RCC_CFGR_MCO_HSI;//RCC_CFGR_MCO_SYSCLK;//RCC_CFGR_MCO_PLL;//
}

uint16_t time_15ms;
uint8_t time_1s,IRrxd[4],IRrxd_start,IRrxd_byte_cnt,IRrxd_bit_cnt,IRrxd_bit_level;
uint32_t wholePulseLength,lowPulseLength;
int main(void)
{
   uint8_t light;
	//PLL_Config();
	
	//LED2-PA5
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;  //打开Port A时钟
	GPIOA->MODER &= (~GPIO_MODER_MODER5);
	GPIOA->MODER |=GPIO_MODER_MODER5_0; //PA5输出
	GPIOA->OTYPER &= (~GPIO_OTYPER_OT_5); //PA5推挽输出
	//GPIOA->PUPDR &= (~GPIO_PUPDR_PUPDR5);
	//GPIOA->PUPDR |= GPIO_PUPDR_PUPDR5_0; //PA5 PULL-UP
	
	MCO_config();
	
	TIM_config();

	time_15ms=0;
	time_1s=0;
   
   IRrxd_start=0;
   
	while(1)
	{
   #if 0
		if(time_1s==2)time_1s=0;
		if(time_1s==0)
		{
			//GPIOA->ODR |= GPIO_ODR_5; //PA5=1
			SET_LED2;
			CLR_SET_LED2_BIT;
		}
		else
		{
			//GPIOA->ODR &= ~GPIO_ODR_5; //PA5=0
			RESET_LED2;
			CLR_RESET_LED2_BIT;
		}
   #endif
      if(IRrxd[0]==0x61&&IRrxd[1]==0xD6)
      {
         if(IRrxd[2]^IRrxd[3]==0xFF)
         {
            switch(IRrxd[2])
            {
               case 0x48:
                  if(light==0)light=1;
                  else light=0;
                  break;
               case 0xA0:
                  break;
               case 0xD8:
                  break;
               case 0x58:
                  break;
               case 0x20:
                  break;
               case 0x60:
                  break;     				 				  				   				 								   				  				
            }
         }         
      }
      if(light==1)
      {
			SET_LED2;
			CLR_SET_LED2_BIT;
      }
      else
      {
			RESET_LED2;
			CLR_RESET_LED2_BIT;
      }
	}
}


void TIM15_IRQHandler(void)
{
	if(TIM15->SR & TIM_SR_UIF)
	{
		TIM15->SR &= ~TIM_SR_UIF;
		time_15ms++;
		if(time_15ms>=67)
		{
			time_15ms=0;
			time_1s++;
		}
      
      IRrxd_start=0;
      IRrxd_byte_cnt=0;
      IRrxd_bit_cnt=0;
      IRrxd_bit_level=255;      
	}
   if(TIM15->SR & TIM_SR_CC1IF)//下降沿，周期
   {
      wholePulseLength=TIM15->CCR1;
      TIM15->SR &= ~TIM_SR_CC1IF;
      if(IRrxd_start==0)
      {
         //接收起始码
         if(lowPulseLength>=64&&lowPulseLength<=80)//低电平范围8~10ms，in 8MHz；
         {
            if(wholePulseLength>=96&&wholePulseLength<=120)//周期范围12~15ms，in 8MHz；
            {
               IRrxd_start=1;
               IRrxd_byte_cnt=0;
               IRrxd_bit_cnt=0;
               IRrxd_bit_level=255;
            }
         }
      }
      else
      {
         //接收逻辑位
         if(lowPulseLength>=3&&lowPulseLength<=7)//低电平范围0.4~0.8ms，in 8MHz；
         {
            if(wholePulseLength>=6&&wholePulseLength<=13)//周期范围0.8~1.6ms，in 8MHz；
            {
               //逻辑0
               IRrxd_bit_level=0;
               IRrxd_bit_cnt++;
            }
            else if(wholePulseLength>=12&&wholePulseLength<=24)//周期范围1.6~3.0ms，in 8MHz；
            {
               //逻辑1
               IRrxd_bit_level=1;
               IRrxd_bit_cnt++;
            }
            else //错误信号
            {
               IRrxd_start=0;
               IRrxd_bit_level=255;
            }
         }
         else //错误信号
         {
            IRrxd_start=0;
            IRrxd_bit_level=255;
         }
         
         if(IRrxd_bit_level!=255)//接收到正确的逻辑位
         {
            if(IRrxd_byte_cnt<=3)//4字节
            {
               if(IRrxd_bit_cnt<=8)
               {				
                  IRrxd[IRrxd_byte_cnt]|=IRrxd_bit_level<<(8-IRrxd_bit_cnt);	
               }
               if(IRrxd_bit_cnt>=8)			//one byte end
               {
                  IRrxd_byte_cnt++;		
                  IRrxd_bit_cnt=0;
               }	               
            }
         }
      }
   }
   if(TIM15->SR & TIM_SR_CC2IF)//上升沿，低电平宽度
   {
      lowPulseLength=TIM15->CCR2;
      TIM15->SR &= ~TIM_SR_CC2IF;
      if(IRrxd_byte_cnt>=3)
      {
         //一帧数据接收完，重新开始接收数据
         IRrxd_start=0;
         IRrxd_byte_cnt=0;
         IRrxd_bit_cnt=0;
         IRrxd_bit_level=255;               
      }
   }   
}
