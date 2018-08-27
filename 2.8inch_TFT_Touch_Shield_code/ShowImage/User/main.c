#include "lcd.h"
#include "delay.h"
#include "main.h"
#include "touch.h"
#include "ff.h"		
#include "fatfs_storage.h"
#include <stdlib.h>
#include <stdio.h>

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
    
    //TP_CS: PB5  TP_IRQ: PB3
    //PB5: 输出，推挽，上拉，高速
    GPIOB->MODER |= GPIO_MODER_MODER5_0;
    GPIOB->OTYPER &= ~GPIO_OTYPER_OT_5;
    GPIOB->PUPDR |= GPIO_PUPDR_PUPDR5_0;
    GPIOB->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR5;
    //PB3: 输入，推挽，上拉，高速
    GPIOB->MODER &= ~GPIO_MODER_MODER3;
    GPIOB->OTYPER &= ~GPIO_OTYPER_OT_3;
    GPIOB->PUPDR |= GPIO_PUPDR_PUPDR3_0;
    GPIOB->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR3;
    
    //SD_CS: PB4
    //PB4: 输出，推挽，上拉，高速
    GPIOB->MODER |= GPIO_MODER_MODER4_0;
    GPIOB->OTYPER &= ~GPIO_OTYPER_OT_4;
    GPIOB->PUPDR |= GPIO_PUPDR_PUPDR4_0;
    GPIOB->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR4;
    TP_CS_SET;//与SD卡通信，TP_CS需要置高
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
    接收阈值：8bit；(触摸屏，读取xpt2046，必须按8位读取；而TFT屏，不需要读取它的返回值，所以不用考虑此位)
    ******************************************/   
    SPI1->CR2 = SPI_CR2_FRXTH | SPI_CR2_DS_2 | SPI_CR2_DS_1 | SPI_CR2_DS_0;

    SPI1->CR1 |= SPI_CR1_SPE;
}


char* pDirectoryFiles[MAX_BMP_FILES];
FATFS microSDFatFs;
uint8_t str[20];

void SDCard_Config(void)
{
  uint32_t counter = 0;
  
  /* Check the mounted device */
  if(f_mount(&microSDFatFs, (TCHAR const*)"/", 0) != FR_OK)
  {
      lcd_display_string(0, 16, "FATFS_NOT_MOUNTED", 16, RED);
  }  
  else
  {
    /* Initialize the Directory Files pointers (heap) */
    for (counter = 0; counter < MAX_BMP_FILES; counter++)
    {
      pDirectoryFiles[counter] = malloc(11); 
    }
  }
}

static void Display_Images(void)
{    
    uint32_t bmplen = 0x00;
    uint32_t checkstatus = 0x00;
    uint32_t filesnumbers = 0x00;
    uint32_t bmpcounter = 0x00;
    DIR directory;
    FRESULT res;
   
    /* Open directory */
    res= f_opendir(&directory, "/");
    if((res != FR_OK))
    {
      if(res == FR_NO_FILESYSTEM)
      {
        /* Display message: SD card not FAT formated */
        lcd_display_string(0, 32, "SD_CARD_NOT_FORMATTED", 16, RED);  
          
      }
      else
      {
        /* Display message: Fail to open directory */
         lcd_display_string(0, 48, "SD_CARD_OPEN_FAIL", 16, RED);           
      }
    }
    
    /* Get number of bitmap files */
    filesnumbers = Storage_GetDirectoryBitmapFiles ("/", pDirectoryFiles);    
    /* Set bitmap counter to display first image */
    bmpcounter = 1; 
    
    while (1)
    {     
        sprintf((char*)str, "%-11.11s", pDirectoryFiles[bmpcounter -1]);
        
        checkstatus = Storage_CheckBitmapFile((const char*)str, &bmplen);
        
        if(checkstatus == 0)
        {
          /* Format the string */
          Storage_OpenReadFile(0, 0, (const char*)str); 
        }
        else if (checkstatus == 1)
        {
          /* Display message: SD card does not exist */
           lcd_display_string(0, 64, "SD_CARD_NOT_FOUND", 16, RED);  
        }
        else
        {
          /* Display message: File not supported */
            lcd_display_string(0, 80, "SD_CARD_FILE_NOT_SUPPORTED", 16, RED); 
        }
        
        bmpcounter ++;
        if(bmpcounter > filesnumbers)
        {
          bmpcounter = 1;
        }
        
        delay_ms(1000);
        delay_ms(1000);
        delay_ms(1000);
    }
}


int main(void)
{
    SystemInit();
    SystemCoreClockUpdate();
    delay_init(SystemCoreClock);

    port_init();
    spi1_init();
    
    lcd_init();
    xpt2046_init();

    tp_adjust();
    tp_dialog();
    TP_CS_SET;
    SDCard_Config();
    
	while(1)
	{
        tp_draw_board();
        Display_Images();
	}
}

