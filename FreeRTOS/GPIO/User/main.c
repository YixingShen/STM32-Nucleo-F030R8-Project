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

#if (configSUPPORT_STATIC_ALLOCATION ==1 )
TaskHandle_t LedTaskHandle;
TaskHandle_t ButtonTaskHandle;
TaskHandle_t FlashTaskHandle;

#define LED_STACK_SIZE  10
StaticTask_t LedTaskTCB;
StackType_t LedTaskStack[LED_STACK_SIZE];

#define BUTTON_STACK_SIZE  15
StaticTask_t ButtonTaskTCB;
StackType_t ButtonTaskStack[BUTTON_STACK_SIZE];

#define FLASH_STACK_SIZE  20
StaticTask_t FlashTaskTCB;
StackType_t FlashTaskStack[FLASH_STACK_SIZE];
#endif
int main(void)
{
    TaskHandle_t ledHandle = NULL;
    TaskHandle_t buttonHandle = NULL;
    TaskHandle_t flashHandle = NULL;
    
    if(SystemCoreClock==48000000)PLL_Config();//SystemCoreClock设置要于此对应
    led_init();
    button_init();
	light=read_flash_HalfWord(0);
    
    //创建任务
#if (configSUPPORT_STATIC_ALLOCATION ==1 )
    LedTaskHandle = xTaskCreateStatic( led_Serve, 
                                       (char const*)"led", 
                                       LED_STACK_SIZE, 
                                       NULL, 
                                       1, 
                                       LedTaskStack, 
                                       &LedTaskTCB);
    ButtonTaskHandle = xTaskCreateStatic( button_Serve, 
                                          (char const*)"button", 
                                          BUTTON_STACK_SIZE, NULL, 
                                          2, 
                                          ButtonTaskStack, 
                                          &ButtonTaskTCB);
    FlashTaskHandle = xTaskCreateStatic( flash_Serve, 
                                         (char const*)"flash", 
                                         FLASH_STACK_SIZE, 
                                         NULL, 
                                         1, 
                                         FlashTaskStack, 
                                         &FlashTaskTCB);
#else    
	xTaskCreate(led_Serve, (char const*)"led", 10, NULL, 1, &ledHandle);//NULL);	
	xTaskCreate(button_Serve, (char const*)"button", 10, NULL, 2, &buttonHandle);//NULL);
    xTaskCreate(flash_Serve, (char const*)"flash", 20, NULL, 1, &flashHandle);//NULL);
#endif
	//启动调度器
	vTaskStartScheduler();

	//正常情况下，不会执行到这里，否则，说明任务沒有创建成功
	while(1){}
}


#if (configSUPPORT_STATIC_ALLOCATION ==1 )
/* 自定义函数，位空闲任务定义任务控制块和堆栈 */
StaticTask_t IdleTaskTCB;
StackType_t IdleTaskStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, 
                                    StackType_t **ppxIdleTaskStackBuffer, 
                                    uint32_t *pulIdleTaskStackSize )
{
    *ppxIdleTaskTCBBuffer=&IdleTaskTCB;
    *ppxIdleTaskStackBuffer=IdleTaskStack;
    *pulIdleTaskStackSize=configMINIMAL_STACK_SIZE;
}

#if (configUSE_TIMERS == 1)
//如果使用软件定时器，需要定义此函数
StaticTask_t TimerTaskTCB;
StackType_t TimerTaskStack[configTIMER_TASK_STACK_DEPTH];

void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, 
                                     StackType_t **ppxTimerTaskStackBuffer, 
                                     uint32_t *pulTimerTaskStackSize )
{
    *ppxTimerTaskTCBBuffer=&TimerTaskTCB;
    *ppxTimerTaskStackBuffer=TimerTaskStack;
    *pulTimerTaskStackSize=configTIMER_TASK_STACK_DEPTH;
}
#endif
#endif
