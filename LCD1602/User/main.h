#include "stm32f0xx.h"

#define LED2_ON     GPIOA->ODR |= GPIO_ODR_5
#define LED2_OFF    GPIOA->ODR &= ~GPIO_ODR_5
#define LED2_FLASH  GPIOA->ODR ^= GPIO_ODR_5

//USER BUTTON
#define KEY_PRESS   (GPIOC->IDR & GPIO_IDR_13)==0
#define KEY_RELEASE (GPIOC->IDR & GPIO_IDR_13)!=0

//V0
//#define V0_H     GPIOB->ODR |= GPIO_ODR_0
//#define V0_L     GPIOB->ODR &= ~GPIO_ODR_0
//RS
#define RS_H     GPIOC->ODR |= GPIO_ODR_1
#define RS_L     GPIOC->ODR &= ~GPIO_ODR_1
//RW
#define RW_H     GPIOC->ODR |= GPIO_ODR_0
#define RW_L     GPIOC->ODR &= ~GPIO_ODR_0
//E
#define E_H      GPIOB->ODR |= GPIO_ODR_7
#define E_L      GPIOB->ODR &= ~GPIO_ODR_7
//D0
#define D0_H     GPIOA->ODR |= GPIO_ODR_1
#define D0_L     GPIOA->ODR &= ~GPIO_ODR_1
//D1
#define D1_H     GPIOC->ODR |= GPIO_ODR_14
#define D1_L     GPIOC->ODR &= ~GPIO_ODR_14
//D2
#define D2_H     GPIOC->ODR |= GPIO_ODR_15
#define D2_L     GPIOC->ODR &= ~GPIO_ODR_15
//D3
#define D3_H     GPIOF->ODR |= GPIO_ODR_0
#define D3_L     GPIOF->ODR &= ~GPIO_ODR_0
//D4
#define D4_H     GPIOF->ODR |= GPIO_ODR_1
#define D4_L     GPIOF->ODR &= ~GPIO_ODR_1
//D5
#define D5_H     GPIOA->ODR |= GPIO_ODR_4
#define D5_L     GPIOA->ODR &= ~GPIO_ODR_4
//D6
#define D6_H     GPIOC->ODR |= GPIO_ODR_2
#define D6_L     GPIOC->ODR &= ~GPIO_ODR_2
//D7
#define D7_H     GPIOC->ODR |= GPIO_ODR_3
#define D7_L     GPIOC->ODR &= ~GPIO_ODR_3

