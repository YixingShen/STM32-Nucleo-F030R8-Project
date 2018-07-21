#include "PL1167.h"

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
   SPI1_NSS:  PA4
	 SPI1_SCK:  PB0 //不能用PB3，PB3用作了SWO
   SPI1_MISO: PB4 //input
   SPI1_MOSI: PB5
	 PKT	PA1  //input
   RFRST PA0
   *******************************************************************/
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;  //打开Port A时钟
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;  //打开Port B时钟

	GPIOA->MODER &= ~GPIO_MODER_MODER1;//PA1输入
	GPIOA->MODER |= GPIO_MODER_MODER4_0 | GPIO_MODER_MODER0_0;//PA4输出，PA0输出
	GPIOB->MODER &= ~GPIO_MODER_MODER4;//PB4输入
	GPIOB->MODER |= GPIO_MODER_MODER5_0 | GPIO_MODER_MODER0_0;//PB5输出，PB0输出

	GPIOA->OTYPER &= ~(GPIO_OTYPER_OT_4 | GPIO_OTYPER_OT_1 | GPIO_OTYPER_OT_0);//PA4 PA1 PA0推挽输出
	//GPIOA->PUPDR |= GPIO_PUPDR_PUPDR4_1 | GPIO_PUPDR_PUPDR1_1 | GPIO_PUPDR_PUPDR0_1;//PA4 PA1 PA0下拉
	GPIOA->PUPDR |= GPIO_PUPDR_PUPDR4_0 | GPIO_PUPDR_PUPDR1_0 | GPIO_PUPDR_PUPDR0_0;//PA4 PA1 PA0上拉

	GPIOB->OTYPER &= ~(GPIO_OTYPER_OT_5 | GPIO_OTYPER_OT_4 | GPIO_OTYPER_OT_0);//PB4 PB5 PB0推挽输出
	//GPIOB->PUPDR |= GPIO_PUPDR_PUPDR5_1 | GPIO_PUPDR_PUPDR4_1 | GPIO_PUPDR_PUPDR0_1;//PB4 PB5 PB0下拉
	GPIOB->PUPDR |= GPIO_PUPDR_PUPDR5_0 | GPIO_PUPDR_PUPDR4_0 | GPIO_PUPDR_PUPDR0_0;//PB4 PB5 PB0上拉
}

int main(void)
{
	uint8_t key_press_cnt;
	
	/*Configure the SysTick to have interrupt in 1ms time basis*/
	SysTick_Config(8000);//使用HSI=8MHz作为系统时钟

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
