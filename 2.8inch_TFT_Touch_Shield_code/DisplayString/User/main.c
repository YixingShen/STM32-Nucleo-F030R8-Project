#include "lcd.h"
#include "delay.h"
#include "main.h"
#if 0
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
#endif

void port_init(void)
{
    //打开Port A B C时钟
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOBEN | RCC_AHBENR_GPIOCEN;
    
    //PA5: SPI1_SCK   PA6: SPI1_MISO   PA7: SPI1_MOSI
    GPIOA->MODER |=  GPIO_MODER_MODER5_1 | GPIO_MODER_MODER6_1 | GPIO_MODER_MODER7_1;//PA5 PA6 PA7复用功能AF0
    GPIOA->AFR[0] &= 0x000FFFFF;
    GPIOA->OTYPER &= ~(GPIO_OTYPER_OT_5 | GPIO_OTYPER_OT_6 | GPIO_OTYPER_OT_7);//PA5 PA6 PA7推挽输出
    GPIOA->PUPDR |= GPIO_PUPDR_PUPDR5_0 | GPIO_PUPDR_PUPDR6_0 | GPIO_PUPDR_PUPDR7_0;//PA5 PA6 PA7上拉
	GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEEDR5_0 | GPIO_OSPEEDR_OSPEEDR6_0 | GPIO_OSPEEDR_OSPEEDR7_0;//PA5 PA6 PA7 Medium 10 MHz

    //LCD_CS: PB6  LCD_BL: PC7  LCD_DC: PA8
    //PB6: 输出，推挽，上拉，高速
    GPIOB->MODER |= GPIO_MODER_MODER6_0;
    GPIOB->OTYPER &= ~GPIO_OTYPER_OT_6;
    GPIOB->PUPDR |= GPIO_PUPDR_PUPDR6_0;
    GPIOB->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR6;
    //PC7: 输出，推挽，上拉，高速
    GPIOC->MODER |= GPIO_MODER_MODER7_0;
    GPIOC->OTYPER &= ~GPIO_OTYPER_OT_7;
    GPIOC->PUPDR |= GPIO_PUPDR_PUPDR7_0;
    GPIOC->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR7;    
    //PA8: 输出，推挽，上拉，高速
    GPIOA->MODER |= GPIO_MODER_MODER8_0;
    GPIOA->OTYPER &= ~GPIO_OTYPER_OT_8;
    GPIOA->PUPDR |= GPIO_PUPDR_PUPDR8_0;
    GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR8; 
}
    
void spi1_init(void)
{
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;  //打开SPI1时钟

    //复位SPI1
    RCC->APB2RSTR |= RCC_APB2RSTR_SPI1RST;
    RCC->APB2RSTR &= ~RCC_APB2RSTR_SPI1RST;   
   
    /******************************************
    空闲状态SCK高电平，第二个SCK边沿采样数据；
    主模式；
    Baud Rate：fpclk/4，fpclk=fhclk=fsysclk=8MHz；
    MSB在前；
    NSS软件管理：
    2线全双工；
    不使用CRC；
    ******************************************/
    SPI1->CR1 = SPI_CR1_MSTR | SPI_CR1_CPOL | SPI_CR1_CPHA | SPI_CR1_BR_0 | SPI_CR1_SSM | SPI_CR1_SSI;
    /******************************************
    不使用TX和RX DMA；
    NSS输出不使能；
    不使用NSS脉冲模式；
    使用Motorola模式；
    错误中断不使能；
    RXNE中断不使能；
    TXE中断不使能；
    数据长度：8bit；
    接收阈值：16bit；
    ******************************************/   
    SPI1->CR2 = SPI_CR2_DS_2 | SPI_CR2_DS_1 | SPI_CR2_DS_0;

    SPI1->CR1 |= SPI_CR1_SPE;
}

int main(void)
{
	/*Configure the SysTick to have interrupt in 1ms time basis*/
	//SysTick_Config(8000);//使用HSI=8MHz作为系统时钟
    
    SystemInit();
    SystemCoreClockUpdate();
    delay_init(SystemCoreClock);

    port_init();
    spi1_init();
    
    lcd_init();
    delay_ms(1000);//delay(1000);
	lcd_clear_screen(RED);
	delay_ms(1000);//delay(1000);
	lcd_clear_screen(GREEN);
	delay_ms(1000);//delay(1000);
	lcd_clear_screen(BLUE);
	delay_ms(1000);//delay(1000);
    lcd_clear_screen(WHITE);

    lcd_display_string(60, 120, (const uint8_t *)"Hello, world !", FONT_1608, BLUE);
    lcd_display_string(30, 152, (const uint8_t *)"2.8' TFT Touch Shield", FONT_1608, GREEN);
//lcd_display_char(0,0,'A',FONT_1608,BLUE);
//lcd_display_char(0,16,'A',FONT_1608,BLUE);
//lcd_display_char(0,32,'A',FONT_1608,BLUE);
//lcd_display_char(0,48,'A',FONT_1608,BLUE);
	while(1)
	{
        //lcd_draw_point(120, 160, RED);
		//LED2_FLASH;	
        //delay_ms(1000);        
		//delay(20);
	}
}

#if 0
/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
   uwTick++;
}
#endif
