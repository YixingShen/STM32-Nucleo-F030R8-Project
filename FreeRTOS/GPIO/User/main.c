#include "main.h"
#include "FreeRTOS.h"
#include "task.h"

extern FLASH_Status write_flash_HalfWord(uint32_t flash_addr, uint16_t data);
extern uint16_t read_flash_HalfWord(uint32_t flash_addr);

extern uint32_t SystemCoreClock;

uint16_t light,key_press_cnt,flag_key_press;

void PLL_Config(void)//configuration PLL as system clock =48MHz
{
	RCC->CFGR |= RCC_CFGR_PLLMUL12;
	RCC->CR |= RCC_CR_PLLON;
	while((RCC->CR&RCC_CR_PLLRDY)==0);
	RCC->CFGR |= RCC_CFGR_SW_PLL;
	while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);	
}

void led_init(void)
{
	//LED2-PA5
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;  //打开Port A时钟
	GPIOA->MODER &= (~GPIO_MODER_MODER5);
	GPIOA->MODER |=GPIO_MODER_MODER5_0; //PA5输出
	GPIOA->OTYPER &= (~GPIO_OTYPER_OT_5); //PA5推挽输出
	//GPIOA->PUPDR &= (~GPIO_PUPDR_PUPDR5);
	//GPIOA->PUPDR |= GPIO_PUPDR_PUPDR5_0; //PA5 PULL-UP    
}

void button_init(void)
{
	//USER_BUTTON-PC13
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN; //打开PORT C时钟
	GPIOC->MODER &= (~GPIO_MODER_MODER13); //PC13设为输入    
}


void led_Serve(void* pvParam)
{	
	while(1)
	{
        /*
		SET_LED2;
		vTaskDelay(500);
        CLR_LED2;
        vTaskDelay(1500);
        */
        if(light==1) SET_LED2;
        else CLR_LED2;
	}    
}

void button_Serve(void* pvParam)
{	
    while(1)
    {
        if(KEY_PRESS)//按键按下
        {
            if(flag_key_press==0)
            {
                if(key_press_cnt>3)
                {
                    key_press_cnt=0;
                    flag_key_press=1;
                    if(light==0)light=1;
                    else light=0;
                    //write_flash_HalfWord(0,light);
                }
                else key_press_cnt++;
            }
        }			
        else 
        {
            key_press_cnt=0;
            flag_key_press=0;
        }
        vTaskDelay(20);
    }
}

void flash_Serve(void* pvParam)
{	
	while(1)
	{
        if(flag_key_press==1)write_flash_HalfWord(0,light);
	}    
}

int main(void)
{
    if(SystemCoreClock==48000000)PLL_Config();//SystemCoreClock设置要于此对应
    led_init();
    button_init();
	light=read_flash_HalfWord(0);
    
    //创建任务
	xTaskCreate(led_Serve, (char const*)"led", 100, NULL, 1, NULL);	
	xTaskCreate(button_Serve, (char const*)"button", 100, NULL, 2, NULL);
    xTaskCreate(flash_Serve, (char const*)"flash", 100, NULL, 1, NULL);

	//启动调度器
	vTaskStartScheduler();

	//正常情况下，不会执行到这里，否则，说明任务沒有创建成功
	while(1){}
}
