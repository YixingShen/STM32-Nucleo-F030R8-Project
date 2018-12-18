#include "main.h"

uint8_t timer_1ms,flag_pwm_change;

__IO uint32_t uwTick;

#define ENCODER     11    // 编码器线数
#define SPEEDRATIO  30    // 电机减速比
#define PPR         (SPEEDRATIO*ENCODER*4) // Pulse/r 每圈可捕获的脉冲数
__IO uint8_t  start_flag=0;
__IO uint16_t time_count=0;        // 时间计数，每1ms增加一(与滴定时器频率有关)
__IO int32_t CaptureNumber=0;      // 输入捕获数

void delay(__IO uint32_t delay_cnt);//delay_cnt in 1ms
void Key_process(void);


#ifdef UART_CONTROL
typedef struct {
  __IO uint8_t IS_Enable ;  	// 电机旋转状态
  __IO int16_t Duty_Cycles ;    // 电机占空比
}MotorParam_TypeDef;
MotorParam_TypeDef DCMotor_Param;

/**
  * 函数功能: 获取电机转动所需要的参数
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */
void Get_Para()
{
    __IO char ch = 0;
    __IO int Num = 0;

    printf("/*------ 直流有刷电机串口控制例程 -----*/\r\n");
    printf("/*------ 输入电机转动参数:(以回车键结束)*/\r\n");
//    printf("      Y --> 启动      N --> 停止    \n");
//    scanf("%c",&ch);
//    /* 启动转动 */
//    if((ch=='y')||(ch=='Y'))
//    {
//        DCMotor_Param.IS_Enable = 1;
//        printf("Y--> 启动转动 \r\n");
//    }
//    else if((ch=='n')||(ch=='N'))
//    {
//        DCMotor_Param.IS_Enable = 0;
//        printf("N--> 停止转动 \r\n");
//        return ;
//    }
//    else return;    
    if(DCMotor_Param.IS_Enable==0)
    {
        printf("      Y --> 启动     \r\n");
        scanf("%c",&ch);
        printf("Your input is %c\r\n",ch);
        /* 启动转动 */
        if((ch=='y')||(ch=='Y'))
        {
            DCMotor_Param.IS_Enable = 1;
            printf("Y--> 启动转动 \r\n");
        }
        else return;
    }
    else if(DCMotor_Param.IS_Enable==1)
    {
        printf("      N --> 停止    \r\n");
        scanf("%c",&ch);
        printf("Your input is %c\r\n",ch);
        /* 停止转动 */
        if((ch=='n')||(ch=='N'))
        {
            DCMotor_Param.IS_Enable = 0;
            printf("N--> 停止转动 \r\n");
            return ;
        }
        else return;
    }
    
    /* 设置占空比 */
    printf("\n------设置输出占空比 x(-2400~2400),正负代表转动方向,并以回车键结束 \r\n");
    scanf("%d",&Num);
    printf("Your input is %d\r\n",Num);
    while((Num > (BDCMOTOR_TIM_PERIOD+1) || (Num <- (BDCMOTOR_TIM_PERIOD+1) )))
    {
        printf("\r\n --速度输入错误,请从新输入( -2400~2400 )\r\n");
        scanf("%d",&Num);
    }
    DCMotor_Param.Duty_Cycles = Num;
    printf(" 占空比设置为:%d/2400 \r\n",DCMotor_Param.Duty_Cycles);

    /* 电机参数一览 */
    if(DCMotor_Param.IS_Enable)
    {
        printf(" 电机状态:启动转动 ------ 输出占空比:%.2f%% \r\n\n",
                (float)DCMotor_Param.Duty_Cycles/(float)(BDCMOTOR_TIM_PERIOD+1)*100.0f);
    }
}
#endif


int main(void)
{
    //__IO char ch = 0,buff[100];
    //float ft;
    
	PLL_Config();

	/*Configure the SysTick to have interrupt in 1ms time basis*/
	SysTick_Config(SystemCoreClock/1000);
	
    LED_init();
    
    KEY_GPIO_Init();
    
#ifdef UART_CONTROL
    USART_GPIO_Init();
    USART_Config();
#endif

    /* 编码器初始化及使能编码器模式 */
    ENCODER_GPIO_Init();
    ENCODER_TIMx_Init();
    
	BDCMOTOR_GPIO_Init();
    BDCMOTOR_TIMx_Init();
    flag_pwm_change=0;
    
#ifdef UART_CONTROL
//    printf("*************************************************************\r\n");
//    printf("*                                                           *\r\n");
//    printf("*  Thank you for using The Development Board Of BBDD ! ^_^  *\r\n");
//    printf("*                                                           *\r\n");
//    printf("*************************************************************\r\n");
    printf("^^^^--请使用硬石上位机收发串口数据--^^^^\r\n");
#endif
	while(1)
	{
#ifdef UART_CONTROL
//        printf("/*------ please input one char:(end with Enter)*/\r\n");
//        scanf("%c",&ch); 
//        printf("Your input is %c\r\n",ch);
//        scanf("%s",buff); 
//        printf("Your input is %s\r\n",buff);
//        scanf("%d",&ch); 
//        printf("Your input is %d\r\n",ch);
//        scanf("%f",&ft); 
//        printf("Your input is %f\r\n",ft);
        LED_ON();
        Get_Para();
        LED_OFF();
        if(DCMotor_Param.IS_Enable)
        {
            /* 设置方向和速度 */
            if(DCMotor_Param.Duty_Cycles>0)
            {
                SetMotorDir(1);
            }
            else
            {
                SetMotorDir(0);
                printf("Duty_Cycles is %d\r\n",DCMotor_Param.Duty_Cycles);
                DCMotor_Param.Duty_Cycles = -DCMotor_Param.Duty_Cycles;
            }
            printf("Duty_Cycles is %d\r\n",DCMotor_Param.Duty_Cycles);
            SetMotorSpeed(DCMotor_Param.Duty_Cycles);
            start_flag = 1;
        }
        else
        {
            /* 关闭通道输出 */
            DCMotor_Param.Duty_Cycles = 0;
            SetMotorSpeed(DCMotor_Param.Duty_Cycles);  
            SetMotorStop();
        }
#else       
        KEY_Scan();
        if(key_id_done==0)Key_process();
        if(PWM_Duty == BDCMOTOR_DUTY_FULL && BDCMOTOR_state == BDCMOTOR_RUN) LED_ON();
        else if(PWM_Duty == BDCMOTOR_DUTY_ZERO && BDCMOTOR_state == BDCMOTOR_RUN) LED_OFF();
        else LED_Toggle();
        while(timer_1ms<TIME_BASE_MAIN);		//20ms
	    timer_1ms=0;
#endif
	}
}

void Key_process(void)
{
    switch(key_id)
    {
        case KEY_USER:
            key_id_done=1;
            switch(BDCMOTOR_state)
            {
                case BDCMOTOR_IDLE:
                    if(BDCMOTOR_Dir==0) BDCMOTOR_Dir=1;
                    else BDCMOTOR_Dir=0;
                    SetMotorDir(BDCMOTOR_Dir);
                    break;
                case BDCMOTOR_RUN:
                    if(flag_pwm_change==0) //pwm +
                    {
                        if(PWM_Duty < BDCMOTOR_DUTY_FULL) PWM_Duty += 100;
                        if(PWM_Duty >= BDCMOTOR_DUTY_FULL) 
                        {
                            PWM_Duty = BDCMOTOR_DUTY_FULL;
                            flag_pwm_change = 1;
                        }
                        SetMotorSpeed(PWM_Duty); 
                    }
                    else if(flag_pwm_change==1) //stop
                    {
                        flag_pwm_change = 2;
                        //PWM_Duty=0;
                        SetMotorStop();         
                    }
                    else //pwm -
                    {
                        if(PWM_Duty > BDCMOTOR_DUTY_ZERO) PWM_Duty -= 100;
                        if(PWM_Duty <= BDCMOTOR_DUTY_ZERO) 
                        {
                            PWM_Duty = BDCMOTOR_DUTY_ZERO;
                            flag_pwm_change = 0;
                            SetMotorStop();        
                        }   
                        SetMotorSpeed(PWM_Duty);             
                    }
                    break;
            }
            break;
    }
}

/**
  * 函数功能: 系统滴答定时器中断回调函数
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 每发生一次滴答定时器中断进入该回调函数一次
  */
void SYSTICK_Callback(void)
{
    if(start_flag) // 等待脉冲输出后才开始计时
    {
        time_count++;         // 每1ms自动增一
        if(time_count==1000)  // 1s
        {
            float Speed = 0;
            CaptureNumber = ( int32_t )(ENCODER_TIMx->CNT)+OverflowCount*65536;
            printf("Inputs:%d \r\n",CaptureNumber);
            // 4 : 使用定时器编码器接口捕获AB相的上升沿和下降沿，一个脉冲*4.
            // 11：编码器线数(转速一圈输出脉冲数)
            // 270：电机减数比，内部电机转动圈数与电机输出轴转动圈数比，即减速齿轮比
            Speed = (float)CaptureNumber/PPR;
            printf("电机实际转动速度%0.2f r/s \r\n",Speed);
     
            if(Speed==0)start_flag = 0;
            OverflowCount = 0;
            ENCODER_TIMx->CNT = 0;
            time_count=0;
        }
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
    
    if(timer_1ms<250)timer_1ms++;
    
    SYSTICK_Callback();
}

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
