#include "stm32f0xx.h"


#define LED2_ON      GPIOA->ODR |= GPIO_ODR_5
#define LED2_OFF     GPIOA->ODR &= ~GPIO_ODR_5
