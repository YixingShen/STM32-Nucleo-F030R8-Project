#include "main.h"

uint8_t rx[6],tx[6],send_size,recv_size;
uint8_t rf_init_step,gReg7_high,gReg7_low;
uint8_t send_cnt,recv_cnt;
uint8_t fun_code,data_code,flag_RFsend;

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
    SPI2_NSS:  PB12
    SPI2_SCK:  PB13   NetCN10_30
    SPI2_MISO: PB14   NetCN10_28
    SPI2_MOSI: PB15   NetCN10_26
    PKT	PA1  //input
    RFRST PA0	 
    *******************************************************************/
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;  //打开Port A时钟
	GPIOA->MODER &= ~GPIO_MODER_MODER1;//PA1输入
	GPIOA->MODER |= GPIO_MODER_MODER0_0;//PA0输出	
	GPIOA->OTYPER &= ~(GPIO_OTYPER_OT_1 | GPIO_OTYPER_OT_0);//PA1 PA0推挽输出
	//GPIOA->PUPDR |= GPIO_PUPDR_PUPDR1_1 | GPIO_PUPDR_PUPDR0_1;//PA1 PA0下拉
	GPIOA->PUPDR |= GPIO_PUPDR_PUPDR1_0 | GPIO_PUPDR_PUPDR0_0;//PA1 PA0上拉
	
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;  //打开Port B时钟
	RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;  //打开SPI2时钟

    GPIOB->MODER |= GPIO_MODER_MODER12_0;//PB12输出
    GPIOB->MODER |=  GPIO_MODER_MODER13_1 | GPIO_MODER_MODER14_1 | GPIO_MODER_MODER15_1;//PB13 PB14 PB15复用功能
    GPIOB->AFR[1] &= 0x000FFFFF;//PB15-AF0 PB14-AF0 PB13-AF0
	GPIOB->OSPEEDR |= GPIO_OSPEEDR_OSPEEDR15 | GPIO_OSPEEDR_OSPEEDR14 | GPIO_OSPEEDR_OSPEEDR12;//PB12 PB14 PB15高速；PB13设为高速，会发送不正常
    GPIOB->OTYPER &= ~(GPIO_OTYPER_OT_15 | GPIO_OTYPER_OT_14 | GPIO_OTYPER_OT_13 | GPIO_OTYPER_OT_12);//PB12 PB13 PB14 PB15推挽输出
	GPIOB->PUPDR |= GPIO_PUPDR_PUPDR15_1 | GPIO_PUPDR_PUPDR14_1 |GPIO_PUPDR_PUPDR13_1 |GPIO_PUPDR_PUPDR12_1;//PB12 PB13 PB14 PB15下拉
	//GPIOB->PUPDR |= GPIO_PUPDR_PUPDR15_0 | GPIO_PUPDR_PUPDR14_0 |GPIO_PUPDR_PUPDR13_0 |GPIO_PUPDR_PUPDR12_0;//PB12 PB13 PB14 PB15上拉
	SPICS_H;//PB12输出高电平
    
    //复位SPI2
    RCC->APB1RSTR |= RCC_APB1RSTR_SPI2RST;
    RCC->APB1RSTR &= ~RCC_APB1RSTR_SPI2RST;   
   
    /******************************************
    空闲状态SCK低电平，第二个SCK边沿采样数据；
    主模式；
    Baud Rate：fpclk/64，fpclk=fhclk=fsysclk=8MHz；
    MSB在前；
    NSS软件管理：PL1167只能用软件管理
    2线全双工；
    不使用CRC；
    ******************************************/
    SPI2->CR1 = SPI_CR1_MSTR | SPI_CR1_CPHA | SPI_CR1_BR_2 | SPI_CR1_BR_0 | SPI_CR1_SSM | SPI_CR1_SSI;
    /******************************************
    不使用TX和RX DMA；
    NSS输出使能；
    不使用NSS脉冲模式；
    使用Motorola模式；
    错误中断不使能；
    RXNE中断使能；
    TXE中断使能；
    数据长度：8bit；
    接收阈值：8bit；
    ******************************************/   
    SPI2->CR2 = SPI_CR2_FRXTH | SPI_CR2_DS_2 | SPI_CR2_DS_1 | SPI_CR2_DS_0 | SPI_CR2_SSOE
              | SPI_CR2_TXEIE; //| SPI_CR2_RXNEIE | SPI_CR2_ERRIE;
    //一旦使能SPI，因为打开了TXEIE中断，就会立即进入该中断；所以在准备好发送的数据之前，不要使能SPI。
    //SPI2->CR1 |= SPI_CR1_SPE;
    //SPICS_L;
	 
	/* Configure NVIC for SPI2 Interrupt */
	//set SPI2 Interrupt to the lowest priority
	NVIC_SetPriority(SPI2_IRQn, 0);
	//Enable Interrupt on SPI1
	NVIC_EnableIRQ(SPI2_IRQn);   
}

//-----------------------------------------------------------------------------
// RF 初始化
//-----------------------------------------------------------------------------
void pdelay (unsigned char t)
{
        while(t!=0)
                t--;
}

void delay_msec(unsigned int x)
{
        unsigned char i,j;
        while(x--)
        {
                i=11,j=50;
                while(i--)
                        while(j--) ;
        }
}
void RegWrite(uint8_t step)
{
    SPI2->CR1 &= ~SPI_CR1_SPE;
    SPICS_H;//发送完一个数据包后，要拉高CS信号，否则，最后获取的寄存器7的值不正常，导致无法正确发射
	send_size=3;
	recv_size=3;  		
	pdelay(35); 
		switch(step)
		{
			case 0:
				tx[0]=0x00;
				tx[1]=0x6f; 
				tx[2]=0xe0;  
				break;
			case 1:
				tx[0]=0x01;
				tx[1]=0x56; 
				tx[2]=0x81;  
				break;		
			case 2:
				tx[0]=0x02;
				tx[1]=0x66; 
				tx[2]=0x17;  
				break;		
			case 3:
				tx[0]=0x04;
				tx[1]=0x9c; 
				tx[2]=0xc9;  
				break;			
			case 4:
				tx[0]=0x05;
				tx[1]=0x66; 
				tx[2]=0x37;  
				break;				
			case 5:
				tx[0]=0x07;
				tx[1]=0x00; 
				tx[2]=0x30;  
				break;		
			case 6:
				tx[0]=0x08;
				tx[1]=0x6c; 
				tx[2]=0x90;  
				break;			
			case 7:
				tx[0]=0x09;
				tx[1]=0x18; 
				tx[2]=0x40;  
				break;				
			case 8:
				tx[0]=0x0a;
				tx[1]=0x7f; 
				tx[2]=0xfd;  
				break;		
			case 9:
				tx[0]=0x0b;
				tx[1]=0x00; 
				tx[2]=0x08;  
				break;		
			case 10:
				tx[0]=0x0c;
				tx[1]=0x00; 
				tx[2]=0x00;  
				break;		
			case 11:
				tx[0]=0x0d;
				tx[1]=0x48; 
				tx[2]=0xbd;  
				break;				
			case 12:
				tx[0]=0x16;
				tx[1]=0x00; 
				tx[2]=0xff;  
				break;			
			case 13:
				tx[0]=0x17;
				tx[1]=0x80; 
				tx[2]=0x05;  
				break;	
			case 14:
				tx[0]=0x18;
				tx[1]=0x00; 
				tx[2]=0x67;  
				break;				
			case 15:
				tx[0]=0x19;
				tx[1]=0x16; 
				tx[2]=0x59;  
				break;				
			case 16:
				tx[0]=0x1a;
				tx[1]=0x19; 
				tx[2]=0xe0;  
				break;				
			case 17:
				tx[0]=0x1b;
				tx[1]=0x13; 
				tx[2]=0x00;  
				break;		
			case 18:
				tx[0]=0x1c;
				tx[1]=0x18; 
				tx[2]=0x00;  
				break;				
			case 19:
				tx[0]=0x20;
				tx[1]=0x48; 
				tx[2]=0x00;  
				break;				
			case 20:
				tx[0]=0x21;
				tx[1]=0x3f; 
				tx[2]=0xc7;  
				break;				
			case 21:
				tx[0]=0x22;
				tx[1]=0x20; 
				tx[2]=0x00;  
				break;				
			case 22:
				tx[0]=0x23;
				tx[1]=0x03; 
				tx[2]=0x80;  
				break;	
	//Begin Syncword					//不同的Syncword可以用以区分不同的设备		
			case 23:
				tx[0]=0x24;
				tx[1]=0x42; 
				tx[2]=0x31;  
				break;	
			case 24:
				tx[0]=0x25;
				tx[1]=0x86; 
				tx[2]=0x75;  
				break;				
			case 25:
				tx[0]=0x26;
				tx[1]=0x9a; 
				tx[2]=0x0b;  
				break;				
			case 26:
				tx[0]=0x27;
				tx[1]=0xde; 
				tx[2]=0xcf;  
				break;				
	//End syncword		
			case 27:
				tx[0]=0x28;
				tx[1]=0x44; 
				tx[2]=0x01;  
				break;			
			case 28:
				tx[0]=0x29;
				tx[1]=0xb0; 
				tx[2]=0x00;  
				break;			
			case 29:
				tx[0]=0x2a;
				tx[1]=0xfd; 
				tx[2]=0xb0;  
				break;	
			case 30:
				tx[0]=0x2b;
				tx[1]=0x00; 
				tx[2]=0x0f;  
				break;
			//获取寄存器 7 的值
			case 31:
                //不能用delay(100)，因为在SPI中断中，无法执行SysTick_Handler中断，会一直卡在该函数中，退不出。
				delay_msec(100);              //delay 100ms to let chip for operation
				tx[0]=0x87;
				tx[1]=0xff; 
				tx[2]=0xff;  				
				break;	
            //初始化完毕，正常发射数据
            case 32:
				tx[0]=0x34;
				tx[1]=0x80; 
				tx[2]=0x00;  	                      
                break;
            case 33:
                tx[0]=0x32;
				tx[1]=0x04;
				tx[2]=(uint8_t)(ADD_INI>>8); 
				tx[3]=(uint8_t)ADD_INI; 
                tx[4]=fun_code;
                tx[5]=data_code;
				send_size=6;
				recv_size=6;              
                break;
            case 34:
				tx[0]=0x07;
				tx[1]=0x01; 
				tx[2]=gReg7_low&0x7f;
                break;          
            case 35:
                while(PKT_IS_LOW) {LED2_ON;}
				LED2_OFF;               
				tx[0]=0x07;
				tx[1]=0x00; 
				tx[2]=gReg7_low&0x7f;
                break;                 
			default:
				tx[0]=0xff;				
				send_size=1;
				recv_size=1;  				
				break;
		}	
    SPICS_L;
    SPI2->CR1 |= SPI_CR1_SPE;
}

void PL1167_Init(void)
{
	RFRST_H;
	delay(10);
	RFRST_L;
	delay(10);
	RFRST_H;               //Enable PL1167
	delay(10);             //delay 5ms to let chip stable
	//RegWrite(rf_init_step);
	send_cnt=0;
	recv_cnt=0;  		
	rf_init_step=0;
	SPICS_L;		
	//使能SPI2
	SPI2->CR1 |= SPI_CR1_SPE;
}

int main(void)
{
    uint8_t key_press_cnt;
    
	/*Configure the SysTick to have interrupt in 1ms time basis*/
	SysTick_Config(8000);//使用HSI=8MHz作为系统时钟

	LED_init();
	Button_init();	
	SPI_init();
	PL1167_Init();
	while(1)
	{
        if(KEY_PRESS&&rf_init_step==32)//初始化完成后，按下按键发射数据
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
		
		if(flag_RFsend)
		{
			flag_RFsend=0;
            SPICS_L;		
            //使能SPI2
            SPI2->CR1 |= SPI_CR1_SPE;     
		}
     
		//LED2_FLASH;						
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

void SPI2_IRQHandler(void)
{
	uint32_t spixbase = 0x00;
	spixbase = (uint32_t)SPI2; 
	spixbase += 0x0C;	
	
	if(send_cnt==0)RegWrite(rf_init_step);

	/* SPI in mode Transmitter -------------------------------------------------*/
	if ((SPI2->SR & SPI_SR_TXE) != RESET)
	{
		*(__IO uint8_t *) spixbase= tx[send_cnt];//SPI2->DR= 0XDF;
        while((SPI2->SR  & SPI_SR_RXNE)==0);//等待接收完一个字节
 		rx[recv_cnt]= *(__IO uint8_t *) spixbase;//SPI2->DR;
		recv_cnt++;
		if(recv_cnt>=recv_size)
        {
            recv_cnt=0;
        }
        
		send_cnt++;
		if(send_cnt>=send_size)
		{
			send_cnt=0;
			rf_init_step++;
            if(rf_init_step==32)
            {
				SPI2->CR1 &= ~SPI_CR1_SPE;
				SPICS_H;
                gReg7_high= rx[1];
                gReg7_low = rx[2];                      
            }
            else if(rf_init_step==36)
            {
                rf_init_step=32;
				SPI2->CR1 &= ~SPI_CR1_SPE;
				SPICS_H;                
            }
		}
	}
#if 0    
	/* SPI in mode Receiver ----------------------------------------------------*/
	if ((SPI2->SR  & SPI_SR_RXNE) != RESET)
	{
		rx[recv_cnt]= *(__IO uint8_t *) spixbase;//SPI2->DR;
		recv_cnt++;
		if(recv_cnt>=recv_size)
        {
            recv_cnt=0;
        }
	}    
#endif
}
