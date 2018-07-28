#ifndef main_h
#define main_h

#include "stm32f0xx.h"

//Address code
#define ADD_INI   0XAF89

//SPI for PL1167
//PKT	input
#define PKT_IS_LOW   (GPIOA->IDR & GPIO_IDR_1)==0//PA1==0
#define PKT_IS_HIGHT (GPIOA->IDR & GPIO_IDR_1)!=0//PA1==1
//RFRST
#define RFRST_H    GPIOA->ODR |= GPIO_ODR_0//PA0=1
#define RFRST_L    GPIOA->ODR &= ~GPIO_ODR_0//PA0=0
//CS
#define SPICS_H       GPIOB->ODR |= GPIO_ODR_12
#define SPICS_L       GPIOB->ODR &= ~GPIO_ODR_12
//#define SPICS_H       SPI2->CR1 |= SPI_CR1_SSI
//#define SPICS_L       SPI2->CR1 &= ~SPI_CR1_SSI

#define LED2_ON     GPIOA->ODR |= GPIO_ODR_5
#define LED2_OFF    GPIOA->ODR &= ~GPIO_ODR_5
#define LED2_FLASH  GPIOA->ODR ^= GPIO_ODR_5

//USER BUTTON
#define KEY_PRESS   (GPIOC->IDR & GPIO_IDR_13)==0
#define KEY_RELEASE (GPIOC->IDR & GPIO_IDR_13)!=0
#endif
