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

void delay2(__IO uint32_t delay_cnt)
{
	while(delay_cnt--);
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

void LCD1602_WriteCmd(uint8_t cmd)//写命令
{
    E_H;
    RS_L;//命令
    RW_L;//写
    if(cmd&0x01)D0_H;
    else D0_L;
    if(cmd&0x02)D1_H;
    else D1_L;
    if(cmd&0x04)D2_H;
    else D2_L;
    if(cmd&0x08)D3_H;
    else D3_H;
    if(cmd&0x10)D4_H;
    else D4_H;
    if(cmd&0x20)D5_H;
    else D5_H;
    if(cmd&0x40)D6_H;
    else D6_L;
    if(cmd&0x80)D7_H;
    else D7_L;
    delay2(10000);
    E_L;       
}
void LCD1602_WriteData(uint8_t addr)
{
    E_H;
    RS_H;//数据
    RW_L;//写
    if(addr&0x01)D0_H;
    else D0_L;
    if(addr&0x02)D1_H;
    else D1_L;
    if(addr&0x04)D2_H;
    else D2_L;
    if(addr&0x08)D3_H;
    else D3_H;
    if(addr&0x10)D4_H;
    else D4_H;
    if(addr&0x20)D5_H;
    else D5_H;
    if(addr&0x40)D6_H;
    else D6_L;
    if(addr&0x80)D7_H;
    else D7_L;
    delay2(1000);
    E_L;    
}

void lcd_dis_self()
{
	unsigned char i = 6;

	while((i < 7) && (i > 1))
	{
        LCD1602_WriteCmd(0x40+i);//自定义字符的第几行
        LCD1602_WriteData(0x1f);//设置自定义字符第几行的内容
        LCD1602_WriteCmd(0x40+0x80);//显示在显示屏上的第二行的第一个
        LCD1602_WriteData(0x0);//显示的是自定义字符的第1个
        delay(500);
		i --;
	}
}

void my_self()
{
			LCD1602_WriteCmd(0x40);//表示设置的是第一个自定义字符

			LCD1602_WriteData(0x06);//显示的是一个电池的样子

			LCD1602_WriteData(0x1f);

			LCD1602_WriteData(0x11);

			LCD1602_WriteData(0x11);

			LCD1602_WriteData(0x11);

			LCD1602_WriteData(0x11);

			LCD1602_WriteData(0x1f);

			LCD1602_WriteData(0x00);

	
			LCD1602_WriteCmd(0x40+0x80);
			LCD1602_WriteData(0x0);
			delay(500);	
			lcd_dis_self();
}
void LCD1602_init(void)
{
    /*********************************************************************
    PIN1: GND
    PIN2: VCC
    PIN3: V0(偏压信号,调对比度;接VCC对比度最弱,接GND时对比度最高)                  PB0
    PIN4: RS(1-数据 0-命令)                       NetCN7_36: PC1
    PIN5: RW(1-读 0-写)                           NetCN7_38: PC0
    PIN6: E(高电平跳变为低电平时，执行命令)       PB7
    PIN7-PIN14数据端口                            
    PIN7:   D0                                    PA1
    PIN8:   D1                                    PC14
    PIN9:   D2                                    PC15
    PIN10:  D3                                    PF0
    PIN11:  D4                                    PF1
    PIN12:  D5                                    PA4
    PIN13:  D6                                    PC2
    PIN14:  D7                                    PC3
    PIN15: 背光正极
    PIN16: 背光负极
    *********************************************************************/
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;  //打开Port A时钟
	GPIOA->MODER |=GPIO_MODER_MODER4_0 | GPIO_MODER_MODER1_0;  //PA1 PA4输出
	GPIOA->OTYPER &= ~(GPIO_OTYPER_OT_4 | GPIO_OTYPER_OT_1);   //PA1 PA4推挽输出
	GPIOA->PUPDR |= GPIO_PUPDR_PUPDR4_0 | GPIO_PUPDR_PUPDR1_0; //PA1 PA4上拉       
    
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;  //打开Port B时钟
	GPIOB->MODER |=GPIO_MODER_MODER7_0 | GPIO_MODER_MODER0_0;  //PB0 PB7输出
	GPIOB->OTYPER &= ~(GPIO_OTYPER_OT_7 | GPIO_OTYPER_OT_0);   //PB0 PB7推挽输出
	GPIOB->PUPDR |= GPIO_PUPDR_PUPDR7_0 | GPIO_PUPDR_PUPDR0_0; //PB0 PB7上拉  
    V0_L;//PB0=0
    
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;  //打开Port C时钟
	GPIOC->MODER |= GPIO_MODER_MODER15_0 | GPIO_MODER_MODER14_0 | GPIO_MODER_MODER3_0 
                  | GPIO_MODER_MODER2_0 | GPIO_MODER_MODER1_0 | GPIO_MODER_MODER0_0;  //PC0 PC1 PC2 PC3 PC14 PC15输出
	GPIOC->OTYPER &= ~(GPIO_OTYPER_OT_15 | GPIO_OTYPER_OT_14 | GPIO_OTYPER_OT_3 
                       | GPIO_OTYPER_OT_2 | GPIO_OTYPER_OT_1 | GPIO_OTYPER_OT_0);   //PC0 PC1 PC2 PC3 PC14 PC15推挽输出
	GPIOC->PUPDR |= GPIO_PUPDR_PUPDR15_0 | GPIO_PUPDR_PUPDR14_0 | GPIO_PUPDR_PUPDR3_0 
                  | GPIO_PUPDR_PUPDR2_0 | GPIO_PUPDR_PUPDR1_0 | GPIO_PUPDR_PUPDR0_0; //PC0 PC1 PC2 PC3 PC14 PC15上拉      

    RCC->AHBENR |= RCC_AHBENR_GPIOFEN;  //打开Port F时钟
	GPIOF->MODER |=GPIO_MODER_MODER1_0 | GPIO_MODER_MODER0_0;  //PF0 PF1输出
	GPIOF->OTYPER &= ~(GPIO_OTYPER_OT_1 | GPIO_OTYPER_OT_0);   //PF0 PF1推挽输出
	GPIOF->PUPDR |= GPIO_PUPDR_PUPDR1_0 | GPIO_PUPDR_PUPDR0_0; //PF0 PF1上拉      
    
    LCD1602_WriteCmd(0x38);//设置工作方式，8位数据接口，两行显示，5*8点阵字符
    delay(5);
    LCD1602_WriteCmd(0x38);
    delay(5);    
    LCD1602_WriteCmd(0x38);
    delay(5);    
    LCD1602_WriteCmd(0x38);
    delay(5);    
    LCD1602_WriteCmd(0x38);
    delay(5);        
    LCD1602_WriteCmd(0x08);//关显示
    delay(5);
    LCD1602_WriteCmd(0x01);//关清屏
    delay(5);
    LCD1602_WriteCmd(0x06);//写入新数据之后光标后移，显示移动
    delay(5);
    LCD1602_WriteCmd(0x0c);//显示开，光标不显示，光标不闪烁
    delay(5);
    my_self();
}

int main(void)
{
	uint8_t key_press_cnt;

	/*Configure the SysTick to have interrupt in 1ms time basis*/
	SysTick_Config(8000);//使用HSI=8MHz作为系统时钟

	LED_init();
	Button_init();	
    LCD1602_init();
    
	while(1)
	{
		if(KEY_PRESS)//按键按下
		{
			if(key_press_cnt>3)
			{
				key_press_cnt=0;
			}
			else key_press_cnt++;
		}			
		else key_press_cnt=0;	        

        LED2_FLASH;
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
