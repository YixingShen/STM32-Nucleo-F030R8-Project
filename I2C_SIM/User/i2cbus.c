/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *  Copyright (c) 1997 by Digital Appliance Controls
 *  Division of Emerson Electric Co.
 *
 *  File name:          I2CBUS.C
 *  Module number:      0802A
 *  Language:           ANSI C
 *
 *  Source code for a i2cbus module is described in more detail in I2cbus.h.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "i2cbus.h"

/*-------------------------*
 |  Private data structure
 *-------------------------*/

/*----------------*
 |  Private data
 *----------------*/

/*-------------------------------*
 |  Private Functions 
 *-------------------------------*/

/* I2C delay function.  Delay execution for a brief peiod of time,
   specified by the parameter I2C_DELAY_COUNT.  If I2C_DELAY_COUNT
   is 0, no delay is performed. */
static void I2cDelay(void) {
	uint8_t delay=I2C_DELAY_COUNT;	
	while(delay!=0) delay--;
	return;
}

/*--------------------*
 |  Public Functions
 *--------------------*/

void I2cStart(void) {
	I2C_CLOCK_H;
	I2cDelay();
	I2C_DATA_OUT;
	GPIOB->ODR |= GPIO_ODR_7;
	I2cDelay();
	GPIOB->ODR &= ~GPIO_ODR_7;
	I2cDelay();
	I2C_CLOCK_L;
	return;
}

void I2cStop(void) {
	I2C_DATA_OUT;	
	GPIOB->ODR &= ~GPIO_ODR_7;		
	I2cDelay();
	I2C_CLOCK_H;
	I2cDelay();
	GPIOB->ODR |= GPIO_ODR_7;
	I2cDelay();
	//I2cDelay();
	//I2cDelay();
	//I2cDelay();
	//I2cDelay();
	//I2C_CLOCK_L;	
}

uint8_t I2cRead(uint8_t doiack) {
	uint8_t i;
	uint8_t out=0;
	/* Read byte first */
	I2C_DATA_IN;	
	for(i=8; i>0; i--) {
		/* Read a data bit by driving the clock line, delaying, reading the bit
		   and dropping the clock line. */
		//MainCheckSignature();
		I2C_CLOCK_H;	
		I2cDelay();
		out<<=1;
		if((GPIOB->IDR&GPIO_IDR_7)!=0) out|=1;
		I2C_CLOCK_L;	
		I2cDelay();
	}	
	/* Now, send ack bit or no-ack bit, depending on the parameter 'doiack'. */
	I2C_DATA_OUT;	
	if(doiack)
	{
		GPIOB->ODR &= ~GPIO_ODR_7;	
	}
	else
	{
		GPIOB->ODR |= GPIO_ODR_7;	
	}
	I2cDelay();
	I2C_CLOCK_H;	
	I2cDelay();
	I2C_CLOCK_L;	
	I2cDelay();
	return out;
}

uint8_t I2cWrite(uint8_t byte) {
	uint8_t m=0x80;
	uint8_t ret=0;
	I2C_DATA_OUT;	
	/* Write byte, MSB first */
	while(m!=0) {
		if(byte & m) {
			GPIOB->ODR |= GPIO_ODR_7;	
		} else {
			GPIOB->ODR &= ~GPIO_ODR_7;	
		}
		I2cDelay();
		//MainCheckSignature();
		I2C_CLOCK_H; 		
		I2cDelay();
		I2C_CLOCK_L;		
		I2cDelay();
		m>>=1;
	}	
	/* Verify ACK sent back. */
	I2cDelay();
	I2C_DATA_IN;	
	//if((GPIOB->IDR&GPIO_IDR_7)==0) ret=1;
	I2cDelay();
	I2C_CLOCK_H;
	if((GPIOB->IDR&GPIO_IDR_7)==0) ret=1;	
	I2cDelay();	
	I2C_CLOCK_L;	
	I2cDelay();
	return ret;
}





