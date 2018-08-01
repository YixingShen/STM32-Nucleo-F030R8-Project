#include "main.h"
#include "1602.h"
#include "string.h"

char const table1[]="LCD1602 check ok\0"; //要显示的第一行内容放入数组table1
char const table2[]="study up\0";         //要显示的第二行内容放入数组table2

char const Heart[]={0x03,0x07,0x0f,0x1f,0x1f,0x1f,0x1f,0x1f, 
                    0x18,0x1E,0x1f,0x1f,0x1f,0x1f,0x1f,0x1f, 
                    0x07,0x1f,0x1f,0x1f,0x1f,0x1f,0x1f,0x1f, 
                    0x10,0x18,0x1c,0x1E,0x1E,0x1E,0x1E,0x1E, 
                    0x0f,0x07,0x03,0x01,0x00,0x00,0x00,0x00, 
                    0x1f,0x1f,0x1f,0x1f,0x1f,0x0f,0x07,0x01, 
                    0x1f,0x1f,0x1f,0x1f,0x1f,0x1c,0x18,0x00, 
                    0x1c,0x18,0x10,0x00,0x00,0x00,0x00,0x00};//心形符

char const ZhenQian[]={0x00,0x00,0x08,0x05,0x00,0x0F,0x02,0x02,
                       0x00,0x00,0x10,0x07,0x04,0x14,0x05,0x05,
                       0x00,0x00,0x01,0x11,0x12,0x12,0x06,0x02, 
                       0x00,0x00,0x02,0x1F,0x02,0x0F,0x02,0x1F,
                       0x00,0x00,0x00,0x1C,0x00,0x18,0x00,0x1C,
                       0x1F,0x02,0x05,0x04,0x08,0x10,0x00,0x00,
                       0x1C,0x04,0x04,0x17,0x14,0x04,0x00,0x00,
                       0x12,0x12,0x12,0x02,0x02,0x02,0x00,0x00,
                       0x08,0x0F,0x08,0x0F,0x08,0x08,0x00,0x00,
                       0x08,0x18,0x08,0x18,0x08,0x18,0x00,0x00};//郑倩

char const ZhenQian_1[]={0x00,0x00,0x08,0x05,0x00,0x0F,0x02,0x02,
                         0x00,0x00,0x10,0x07,0x05,0x15,0x06,0x05,
                         0x00,0x00,0x04,0x07,0x08,0x1B,0x08,0x0F, 
                         0x00,0x00,0x10,0x1C,0x10,0x1C,0x10,0x1E,
                         0x1F,0x02,0x05,0x04,0x08,0x10,0x00,0x00,
                         0x1D,0x06,0x04,0x14,0x04,0x04,0x00,0x00,
                         0x0A,0x0B,0x0A,0x0B,0x0A,0x0A,0x00,0x00,
                         0x04,0x1C,0x04,0x1C,0x04,0x0C,0x00,0x00};//郑倩
                       
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

void LCD1602_WriteCmd(uint8_t cmd)//写命令
{
    E_L;
    RS_L;//命令
    RW_L;//写
    #if 0
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
    #else
    GPIOB->ODR &= 0x00ff;
    GPIOB->ODR |= cmd<<8;
    #endif
    delay(2);
    E_H;
    delay(2);
    E_L;       
}
void LCD1602_WriteData(uint8_t data)
{
    E_L;
    RS_H;//数据
    RW_L;//写
    #if 0
    if(data&0x01)D0_H;
    else D0_L;
    if(data&0x02)D1_H;
    else D1_L;
    if(data&0x04)D2_H;
    else D2_L;
    if(data&0x08)D3_H;
    else D3_H;
    if(data&0x10)D4_H;
    else D4_H;
    if(data&0x20)D5_H;
    else D5_H;
    if(data&0x40)D6_H;
    else D6_L;
    if(data&0x80)D7_H;
    else D7_L;
    #else
    GPIOB->ODR &= 0x00ff;
    GPIOB->ODR |= data<<8;
    #endif
    delay(2);
    E_H;
    delay(2);
    E_L;    
}

void LCD1602_init(void)
{
    /*********************************************************************
    1602工作电压是5V，
    控制引脚和数据引脚需要接STM32F030R8的5V兼容引脚(5V tolerant I/O)，才能使1602正常工作。
    PIN1: GND
    PIN2: VCC
    PIN3: V0(偏压信号,调对比度;接VCC对比度最弱,接GND时对比度最高)
    PIN4: RS(1-数据 0-命令)                       PB5
    PIN5: RW(1-读 0-写)                           PB6
    PIN6: E(高电平跳变为低电平时，执行命令)       PB7
    PIN7-PIN14数据端口                            
    PIN7:   D0                                    PB8
    PIN8:   D1                                    PB9
    PIN9:   D2                                    PB10
    PIN10:  D3                                    PB11
    PIN11:  D4                                    PB12
    PIN12:  D5                                    PB13：NetCN10_30
    PIN13:  D6                                    PB14：NetCN10_28
    PIN14:  D7                                    PB15：NetCN10_26
    PIN15: 背光正极
    PIN16: 背光负极
    *********************************************************************/
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;  //打开Port B时钟
	GPIOB->MODER |= GPIO_MODER_MODER15_0 | GPIO_MODER_MODER14_0 | GPIO_MODER_MODER13_0
                  | GPIO_MODER_MODER12_0 | GPIO_MODER_MODER11_0 | GPIO_MODER_MODER10_0
                  | GPIO_MODER_MODER9_0 | GPIO_MODER_MODER8_0 | GPIO_MODER_MODER7_0
                  | GPIO_MODER_MODER6_0 | GPIO_MODER_MODER5_0;  //PB5~PB15输出
	GPIOB->OTYPER &= 0xffffffe0;   //PB5~PB15推挽输出
	GPIOB->PUPDR |= GPIO_PUPDR_PUPDR15_0 | GPIO_PUPDR_PUPDR14_0 | GPIO_PUPDR_PUPDR13_0
                  | GPIO_PUPDR_PUPDR12_0 | GPIO_PUPDR_PUPDR11_0 | GPIO_PUPDR_PUPDR10_0
                  | GPIO_PUPDR_PUPDR9_0 | GPIO_PUPDR_PUPDR8_0 | GPIO_PUPDR_PUPDR7_0
                  | GPIO_PUPDR_PUPDR6_0 | GPIO_PUPDR_PUPDR5_0; //PB5~PB15上拉  
    
    
    LCD1602_WriteCmd(Function_Set+Data_Interface_8+Double_Line_Display);//设置工作方式，8位数据接口，两行显示，5*8点阵字符
    LCD1602_WriteCmd(Display_OnOff);//关显示
    LCD1602_WriteCmd(Clear_Screen);//清屏
    LCD1602_WriteCmd(Mode_Set+Cursor_Shift_right);//写入新数据之后光标右移，显示不移动
    LCD1602_WriteCmd(Display_OnOff+Display_On);//显示开，光标不显示，光标不闪烁
}

void LCD1602_Display(void)
{
    //DDRAM(Display Data RAM)就是显示数据RAM，用来寄存待显示的字符代码，共80个字节。
    //DDRAM寄存的字符来源有2个：CGROM或CGRAM。
    //显示字符产生器CGROM(Character Generator ROM)中存储的字符
    uint8_t i;

    //第一行显示
    LCD1602_WriteCmd(Set_DDRAM_Address);//设定DDRAM地址，即00H，第一行第一位
    for(i=0;i<strlen(table1);i++)     //将table1[]中的数据依次写入1602显示 
    { 
        LCD1602_WriteData(table1[i]);           
        delay(2); 
    }
    //第二行显示
    LCD1602_WriteCmd(Set_DDRAM_Address+0x40+4);//设定DDRAM地址，即40H，第二行第一位
    for(i=0;i<strlen(table2);i++)     //将table1[]中的数据依次写入1602显示 
    { 
        LCD1602_WriteData(table2[i]);           
        delay(2); 
    }    
}

void LCD1602_CustomDisplay(const char *disp, uint8_t len)
{
    //CGRAM(Character Generator RAM)存放用户自定义的字符
    //设置CGRAM地址指令中，地址为6位，一共可以表示64个地址，即64个字节。
    //一个5×8点阵字符共占用8个字节，那么这64个字节一共可以自定义8个字符。
    //一个5×10点阵字符共占用16个字节，那么这64个字节一共可以自定义4个字符。
    /*--------------------------------|----------------------------------------
    D5D4D3   \D2D1D0                  | 000  001  010  011  100  101  110  111
    (8个字符) \(每个字符的8个字节)    |
    ----------------------------------|----------------------------------------
    000                               |
    ----------------------------------|----------------------------------------
    001                               |
    ----------------------------------|----------------------------------------
    010                               |
    ----------------------------------|----------------------------------------
    011                               |
    ----------------------------------|----------------------------------------
    100                               |
    ----------------------------------|----------------------------------------
    101                               |
    ----------------------------------|----------------------------------------
    110                               |
    ----------------------------------|----------------------------------------
    111                               |
    ----------------------------------|---------------------------------------*/
    uint8_t i;

    LCD1602_WriteCmd(Set_CGRAM_Address);//设置CGRAM地址
    for(i=0;i<64;i++)    
    { 
        LCD1602_WriteData(disp[i]);           
        delay(2); 
    }    
    //只有8个字符：0到7
    //第一行显示 
    LCD1602_WriteCmd(Set_DDRAM_Address+5);
    for(i=0;i<len/16;i++)
    { 
        LCD1602_WriteData(i);           
        delay(2); 
    }
    //第二行显示
    LCD1602_WriteCmd(Set_DDRAM_Address+0x40+5);
    for(i=len/16;i<8;i++)
    { 
        LCD1602_WriteData(i);           
        delay(2); 
    } 
}

uint8_t flag_disp,flag_disp_last;
int main(void)
{
	uint8_t key_press_cnt;

	/*Configure the SysTick to have interrupt in 1ms time basis*/
	SysTick_Config(8000);//使用HSI=8MHz作为系统时钟

	LED_init();
	Button_init();	
    LCD1602_init();
    delay(5);
    //LCD1602_Display();
    
	while(1)
	{
		if(KEY_PRESS)//按键按下
		{
			if(key_press_cnt>3)
			{
				key_press_cnt=0;
                flag_disp++;
                if(flag_disp>=3)flag_disp=0;
			}
			else key_press_cnt++;
		}
		else key_press_cnt=0;	        
        
        if(flag_disp_last!=flag_disp)LCD1602_WriteCmd(Clear_Screen);//清屏
        if(flag_disp==2)LCD1602_CustomDisplay(ZhenQian_1, sizeof(ZhenQian_1));
        else if(flag_disp==1)LCD1602_CustomDisplay(Heart, sizeof(Heart));
        else LCD1602_Display();
            
        flag_disp_last=flag_disp;
        
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

