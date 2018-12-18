#include "main.h"

uint8_t timer_1ms,flag_pwm_change;

__IO uint32_t uwTick;

#define ENCODER     11    // ����������
#define SPEEDRATIO  30    // ������ٱ�
#define PPR         (SPEEDRATIO*ENCODER*4) // Pulse/r ÿȦ�ɲ����������
__IO uint8_t  start_flag=0;
__IO uint16_t time_count=0;        // ʱ�������ÿ1ms����һ(��ζ�ʱ��Ƶ���й�)
__IO int32_t CaptureNumber=0;      // ���벶����

void delay(__IO uint32_t delay_cnt);//delay_cnt in 1ms
void Key_process(void);


#ifdef UART_CONTROL
typedef struct {
  __IO uint8_t IS_Enable ;  	// �����ת״̬
  __IO int16_t Duty_Cycles ;    // ���ռ�ձ�
}MotorParam_TypeDef;
MotorParam_TypeDef DCMotor_Param;

/**
  * ��������: ��ȡ���ת������Ҫ�Ĳ���
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
void Get_Para()
{
    __IO char ch = 0;
    __IO int Num = 0;

    printf("/*------ ֱ����ˢ������ڿ������� -----*/\r\n");
    printf("/*------ ������ת������:(�Իس�������)*/\r\n");
//    printf("      Y --> ����      N --> ֹͣ    \n");
//    scanf("%c",&ch);
//    /* ����ת�� */
//    if((ch=='y')||(ch=='Y'))
//    {
//        DCMotor_Param.IS_Enable = 1;
//        printf("Y--> ����ת�� \r\n");
//    }
//    else if((ch=='n')||(ch=='N'))
//    {
//        DCMotor_Param.IS_Enable = 0;
//        printf("N--> ֹͣת�� \r\n");
//        return ;
//    }
//    else return;    
    if(DCMotor_Param.IS_Enable==0)
    {
        printf("      Y --> ����     \r\n");
        scanf("%c",&ch);
        printf("Your input is %c\r\n",ch);
        /* ����ת�� */
        if((ch=='y')||(ch=='Y'))
        {
            DCMotor_Param.IS_Enable = 1;
            printf("Y--> ����ת�� \r\n");
        }
        else return;
    }
    else if(DCMotor_Param.IS_Enable==1)
    {
        printf("      N --> ֹͣ    \r\n");
        scanf("%c",&ch);
        printf("Your input is %c\r\n",ch);
        /* ֹͣת�� */
        if((ch=='n')||(ch=='N'))
        {
            DCMotor_Param.IS_Enable = 0;
            printf("N--> ֹͣת�� \r\n");
            return ;
        }
        else return;
    }
    
    /* ����ռ�ձ� */
    printf("\n------�������ռ�ձ� x(-2400~2400),��������ת������,���Իس������� \r\n");
    scanf("%d",&Num);
    printf("Your input is %d\r\n",Num);
    while((Num > (BDCMOTOR_TIM_PERIOD+1) || (Num <- (BDCMOTOR_TIM_PERIOD+1) )))
    {
        printf("\r\n --�ٶ��������,���������( -2400~2400 )\r\n");
        scanf("%d",&Num);
    }
    DCMotor_Param.Duty_Cycles = Num;
    printf(" ռ�ձ�����Ϊ:%d/2400 \r\n",DCMotor_Param.Duty_Cycles);

    /* �������һ�� */
    if(DCMotor_Param.IS_Enable)
    {
        printf(" ���״̬:����ת�� ------ ���ռ�ձ�:%.2f%% \r\n\n",
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

    /* ��������ʼ����ʹ�ܱ�����ģʽ */
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
    printf("^^^^--��ʹ��Ӳʯ��λ���շ���������--^^^^\r\n");
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
            /* ���÷�����ٶ� */
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
            /* �ر�ͨ����� */
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
  * ��������: ϵͳ�δ�ʱ���жϻص�����
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ÿ����һ�εδ�ʱ���жϽ���ûص�����һ��
  */
void SYSTICK_Callback(void)
{
    if(start_flag) // �ȴ����������ſ�ʼ��ʱ
    {
        time_count++;         // ÿ1ms�Զ���һ
        if(time_count==1000)  // 1s
        {
            float Speed = 0;
            CaptureNumber = ( int32_t )(ENCODER_TIMx->CNT)+OverflowCount*65536;
            printf("Inputs:%d \r\n",CaptureNumber);
            // 4 : ʹ�ö�ʱ���������ӿڲ���AB��������غ��½��أ�һ������*4.
            // 11������������(ת��һȦ���������)
            // 270����������ȣ��ڲ����ת��Ȧ�����������ת��Ȧ���ȣ������ٳ��ֱ�
            Speed = (float)CaptureNumber/PPR;
            printf("���ʵ��ת���ٶ�%0.2f r/s \r\n",Speed);
     
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
