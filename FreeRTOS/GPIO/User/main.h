#include "stm32f0xx.h"
#include "flash_as_eeprom.h"

#define SET_LED2 GPIOA->ODR |= GPIO_ODR_5
#define CLR_LED2 GPIOA->ODR &= ~GPIO_ODR_5

//USER BUTTON
#define KEY_PRESS   (GPIOC->IDR & GPIO_IDR_13)==0
#define KEY_RELEASE (GPIOC->IDR & GPIO_IDR_13)!=0
