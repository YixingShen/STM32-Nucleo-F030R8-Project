#include "key.h"

void KEY_GPIO_Init(void)
{
	//USER_BUTTON-PC13
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN; //打开PORT C时钟
    GPIOC->PUPDR |= GPIO_PUPDR_PUPDR13_0; //上拉
	GPIOC->MODER &= (~GPIO_MODER_MODER13); //输入
}

uint8_t key_press_timer,key_relse_timer,key_input,key_input_last,key_id,key_id_done;
void KEY_Scan(void)
{
	key_input_last=key_input;
	key_input=0;

    if((GPIOC->IDR & GPIO_IDR_13) == RESET)
        key_input = KEY_USER;
    
	if(key_input>0&&key_input_last==key_input)
	{
		key_press_timer++;
		key_relse_timer=0;
	    if(key_press_timer>3)
	    {
            key_press_timer=100;	   	   	   	
            key_id=key_input;	 	   		
        }	   
	}   
    else
	{
		key_press_timer=0;
		key_relse_timer++;
        if(key_relse_timer>6)
        {
            key_relse_timer=100; 
            key_id_done=0;
            key_id=KEY_NONE;  
        }	   
	}      
}



