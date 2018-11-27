#include "main.h"

uint8_t timer_1ms,flag_pwm_change;

__IO uint32_t uwTick;
void delay(__IO uint32_t delay_cnt);//delay_cnt in 1ms
void Key_process(void);

int main(void)
{
	PLL_Config();

	/*Configure the SysTick to have interrupt in 1ms time basis*/
	SysTick_Config(SystemCoreClock/1000);
	
    LED_init();
    
    KEY_GPIO_Init();
    
	BDCMOTOR_GPIO_Init();
    
    BDCMOTOR_TIMx_Init();
    flag_pwm_change=0;
    
	while(1)
	{
        KEY_Scan();
        if(key_id_done==0)Key_process();
        if(PWM_Duty == BDCMOTOR_DUTY_FULL && BDCMOTOR_state == BDCMOTOR_RUN) LED_ON();
        else if(PWM_Duty == BDCMOTOR_DUTY_ZERO && BDCMOTOR_state == BDCMOTOR_RUN) LED_OFF();
        else LED_Toggle();
        while(timer_1ms<TIME_BASE_MAIN);		//20ms
	    timer_1ms=0;
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
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
    uwTick++;
    
    if(timer_1ms<250)timer_1ms++;
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
