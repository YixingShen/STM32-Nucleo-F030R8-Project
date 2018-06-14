#include "stm32f0xx.h"

#define I2C_ADDRESS        0x30F
#define I2C_ADDRESS_2      0xFE
#define I2C_ADDRESS_2_MSK  0//0-7

#define I2C_SLAVE_ADDRESS  0xA0

#define I2C_NBYTES  2

/* I2C TIMING Register define when I2C clock source is SYSCLK */
/* I2C TIMING is calculated in case of the I2C Clock source is the SYSCLK = 48 MHz */
/* This example use TIMING to 0x00A51314 to reach 1 MHz speed (Rise time = 100 ns, Fall time = 100 ns) */
#define I2C_TIMING      0x00A51314


#define SET_LED2 GPIOA->BSRR |= GPIO_BSRR_BS_5
#define CLR_SET_LED2_BIT GPIOA->BSRR &= ~GPIO_BSRR_BS_5

#define RESET_LED2 GPIOA->BSRR |= GPIO_BSRR_BR_5
#define CLR_RESET_LED2_BIT GPIOA->BSRR &= ~GPIO_BSRR_BR_5
