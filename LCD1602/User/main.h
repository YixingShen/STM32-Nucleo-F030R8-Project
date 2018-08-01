#include "stm32f0xx.h"

#define LED2_ON     GPIOA->ODR |= GPIO_ODR_5
#define LED2_OFF    GPIOA->ODR &= ~GPIO_ODR_5
#define LED2_FLASH  GPIOA->ODR ^= GPIO_ODR_5

//USER BUTTON
#define KEY_PRESS   (GPIOC->IDR & GPIO_IDR_13)==0
#define KEY_RELEASE (GPIOC->IDR & GPIO_IDR_13)!=0


//RS
#define RS_H     GPIOB->ODR |= GPIO_ODR_5
#define RS_L     GPIOB->ODR &= ~GPIO_ODR_5
//RW
#define RW_H     GPIOB->ODR |= GPIO_ODR_6
#define RW_L     GPIOB->ODR &= ~GPIO_ODR_6
//E
#define E_H      GPIOB->ODR |= GPIO_ODR_7
#define E_L      GPIOB->ODR &= ~GPIO_ODR_7
//D0
#define D0_H     GPIOB->ODR |= GPIO_ODR_8
#define D0_L     GPIOB->ODR &= ~GPIO_ODR_8
//D1
#define D1_H     GPIOB->ODR |= GPIO_ODR_9
#define D1_L     GPIOB->ODR &= ~GPIO_ODR_9
//D2
#define D2_H     GPIOB->ODR |= GPIO_ODR_10
#define D2_L     GPIOB->ODR &= ~GPIO_ODR_10
//D3
#define D3_H     GPIOB->ODR |= GPIO_ODR_11
#define D3_L     GPIOB->ODR &= ~GPIO_ODR_11
//D4
#define D4_H     GPIOB->ODR |= GPIO_ODR_12
#define D4_L     GPIOB->ODR &= ~GPIO_ODR_12
//D5
#define D5_H     GPIOB->ODR |= GPIO_ODR_13
#define D5_L     GPIOB->ODR &= ~GPIO_ODR_13
//D6
#define D6_H     GPIOB->ODR |= GPIO_ODR_14
#define D6_L     GPIOB->ODR &= ~GPIO_ODR_14
//D7
#define D7_H     GPIOB->ODR |= GPIO_ODR_15
#define D7_L     GPIOB->ODR &= ~GPIO_ODR_15

