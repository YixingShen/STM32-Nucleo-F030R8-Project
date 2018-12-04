#ifndef __USART_H
#define __USART_H

#include "stm32f0xx.h"
#include <stdio.h>

//115200-8-N-1
#define USART             	 USART1
#define BaudRate	         115200
#define USART_RCC         	 RCC_APB2ENR_USART1EN

//PIN42: PA9---USART1_TX        PIN43: PA10---USART2_RX
#define USART_GPIO_RCC       RCC_AHBENR_GPIOAEN
#define USART_GPIO_PORT      GPIOA  
#define USART_TX_PIN         GPIO_Pin_9  // out
#define USART_RX_PIN         GPIO_Pin_10 // in  


void USART_GPIO_Init(void);
void USART_Config(void);



#endif

