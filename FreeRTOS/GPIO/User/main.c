#include "main.h"
#include "FreeRTOS.h"
#include "task.h"

extern uint32_t SystemCoreClock;

extern FLASH_Status write_flash_HalfWord(uint32_t flash_addr, uint16_t data);
extern uint16_t read_flash_HalfWord(uint32_t flash_addr);
extern void PLL_Config(void);
extern void led_init(void);
extern void button_init(void);

//uint16_t light;
typedef struct LED_Button_Information 
{
    uint8_t flag_blink;
    uint16_t flag_key_press;
    uint16_t light;
}LED_Button_Info;

TaskHandle_t LedBlinkHandle;
extern void led_blink(void* pvParam);
extern void led_Serve(void* pvParam);
extern void button_Serve(void* pvParam);
extern void flash_Serve(void* pvParam);


#define LED_BLINK_STACK_SIZE  20
#define LED_STACK_SIZE  10
#define BUTTON_STACK_SIZE  15
#define FLASH_STACK_SIZE  20

#if (configSUPPORT_STATIC_ALLOCATION ==1 )
StaticTask_t LedTaskTCB;
StackType_t LedTaskStack[LED_STACK_SIZE];

StaticTask_t ButtonTaskTCB;
StackType_t ButtonTaskStack[BUTTON_STACK_SIZE];

StaticTask_t FlashTaskTCB;
StackType_t FlashTaskStack[FLASH_STACK_SIZE];

StaticTask_t LedBlinkTCB;
StackType_t LedBlinkStack[LED_BLINK_STACK_SIZE];
#endif

int main(void)
{
    //uint8_t flag_blink;
    //uint16_t flag_key_press;
    LED_Button_Info run_info;
    TaskHandle_t LedTaskHandle = NULL;
    TaskHandle_t ButtonTaskHandle = NULL;
    TaskHandle_t FlashTaskHandle = NULL;
    
    if(SystemCoreClock==48000000)PLL_Config();//SystemCoreClock设置要于此对应
    led_init();
    button_init();
	//light=read_flash_HalfWord(0);
    run_info.light=read_flash_HalfWord(0);

    
    //创建任务
    /*每个任务都有一段时间来执行，这段时间执行这个任务，下段时间执行另一个任务，
    并不是完全执行完某个任务后才去执行另一个任务，每个任务都有一个时间片来执行；
    优先级的作用并不是完全执行完某个任务后，再执行低优先级的任务，
    而是在低优先级任务执行的时间片内，高优先级任务可以打断它，该时间片的剩余时间来执行高优先级任务，
    时间片结束后，又转去执行下一个任务。*/
#if (configSUPPORT_STATIC_ALLOCATION ==1 )
    //上电后，LED先闪5次；再根据FLASH读取的值或按键操作，常亮或常灭
    LedBlinkHandle = xTaskCreateStatic( led_blink, 
                                        (char const*)"led blink", 
                                        LED_BLINK_STACK_SIZE, 
                                        &run_info,//&flag_blink,//NULL, 
                                        1, 
                                        LedBlinkStack, 
                                        &LedBlinkTCB);   

    LedTaskHandle = xTaskCreateStatic( led_Serve, 
                                       (char const*)"led", 
                                       LED_STACK_SIZE, 
                                       &run_info,//&flag_blink,//NULL, 
                                       1, 
                                       LedTaskStack, 
                                       &LedTaskTCB);
    ButtonTaskHandle = xTaskCreateStatic( button_Serve, 
                                          (char const*)"button", 
                                          BUTTON_STACK_SIZE, 
                                          &run_info,//&flag_key_press,//NULL, 
                                          2, 
                                          ButtonTaskStack, 
                                          &ButtonTaskTCB);
    FlashTaskHandle = xTaskCreateStatic( flash_Serve, 
                                         (char const*)"flash", 
                                         FLASH_STACK_SIZE, 
                                         &run_info,//&flag_key_press,//NULL, 
                                         1, 
                                         FlashTaskStack, 
                                         &FlashTaskTCB);
#else    
    //上电后，LED先闪5次；再根据FLASH读取的值或按键操作，常亮或常灭
    xTaskCreate( led_blink, 
                 (char const*)"led blink", 
                 LED_BLINK_STACK_SIZE, 
                 &run_info,//&flag_blink, 
                 1, 
                 &LedBlinkHandle);
    //xTaskCreate(led_Serve, (char const*)"led", 10, NULL, 1, NULL);	    
    xTaskCreate( led_Serve, 
                 (char const*)"led", 
                 LED_STACK_SIZE, 
                 &run_info,//&flag_blink, 
                 1, 
                 &LedTaskHandle);        
	xTaskCreate( button_Serve, 
                 (char const*)"button", 
                 BUTTON_STACK_SIZE, 
                 &run_info,//&flag_key_press,//NULL, 
                 2, 
                 &ButtonTaskHandle);//NULL);
    xTaskCreate( flash_Serve, 
                 (char const*)"flash", 
                 FLASH_STACK_SIZE, 
                 &run_info,//&flag_key_press,//NULL, 
                 1, 
                 &FlashTaskHandle);//NULL);
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


void led_blink(void* pvParam)
{	
    uint8_t cnt=0;
    //uint8_t* p;
    //p=pvParam;
    //*p=1;
    LED_Button_Info * param;
    param=pvParam;
    param->flag_blink=1;//(*param).flag_blink=1;
	while(1)
	{
        cnt++;
        SET_LED2;
        vTaskDelay(250);
        CLR_LED2;
        vTaskDelay(750);
        if(cnt>=5)
        {
            //*p=0;
            param->flag_blink=0;//(*param).flag_blink=1;
            vTaskDelete(LedBlinkHandle);
        }
	}    
}

void led_Serve(void* pvParam)
{	     
    //uint8_t* p;
    //p=pvParam;    
    LED_Button_Info * param;
    param=pvParam;
	while(1)
	{
        if(param->flag_blink==0)//(*p==0)
        {
            if(param->light==1)//(light==1) 
                SET_LED2;
            else CLR_LED2;
        }
	}  
}

void button_Serve(void* pvParam)
{	
    uint16_t key_press_cnt;
    //uint16_t* p;
    //p=pvParam;
    LED_Button_Info * param;
    param=pvParam;    
    while(1)
    {
        if(KEY_PRESS)//按键按下
        {
            if(param->flag_key_press==0)//(*p==0)
            {
                if(key_press_cnt>3)
                {
                    key_press_cnt=0;
                    param->flag_key_press=1;//*p=1;
                    //if(light==0)light=1;
                    //else light=0;
                    if(param->light==0)param->light=1;
                    else param->light=0;
                }
                else key_press_cnt++;
            }
        }			
        else 
        {
            key_press_cnt=0;
            //*p=0;
            param->flag_key_press=0;
        }
        vTaskDelay(20);
    }
}

void flash_Serve(void* pvParam)
{
    //uint16_t* p;
    //p=pvParam;
    LED_Button_Info * param;
    param=pvParam;   
    
	while(1)
	{
        //if(*p==1) write_flash_HalfWord(0,light);
        if(param->flag_key_press==1) write_flash_HalfWord(0,param->light);
	}
}

