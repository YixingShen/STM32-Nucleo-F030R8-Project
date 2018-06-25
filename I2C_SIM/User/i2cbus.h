/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
*
*  Copyright (c) 1997 by Digital Appliance Controls
*  Division of Emerson Electric Co.
*
*  File Name:          I2CBUS.H
*  Module Number:      0802A
*  Language:           ANSI C
*
*  Defintion
*		Low level functions for bit-banged I2C communications.
*	Error Dectection
*	  The only error detected is when a bus device does not acknowledge a byte
*    sent.
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
*
*  Edit date     Programmer        Ver   Description
*  ---------     ----------        ---   -----------
*  09/23/98      Tom Sheahan        A    Initial release
*                                                                           
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "stm32f0xx.h"

#define I2C_CLOCK_H			GPIOB->ODR |= GPIO_ODR_6
#define I2C_CLOCK_L			GPIOB->ODR &= ~GPIO_ODR_6
#define I2C_DATA_OUT		{GPIOB->MODER |= GPIO_MODER_MODER7_0; \
                         GPIOB->OTYPER |= GPIO_OTYPER_OT_7; \
                         GPIOB->PUPDR |= GPIO_PUPDR_PUPDR7_0;}
#define I2C_DATA_IN 		{GPIOB->MODER &= ~GPIO_MODER_MODER7_0;}



/*-------------------------------
 |  Project compile options
 *------------------------------*/

/* Delay count between clock transitions. */
#define I2C_DELAY_COUNT  100//5

/*---------------------------
  Type definitions
---------------------------*/

/*--------------------*
 |  Public functions
 *--------------------*/

/*---------------------------------------------------------------------------*
 |  I2cStart()
 |
 |  Prepare the bus for data transfer by driving the clock and data lines
 |  to the necessary states signifying a start condition
 |
 |  Entry:  None
 |  Exit:   None   
 *---------------------------------------------------------------------------*/
void I2cStart(void);

/*---------------------------------------------------------------------------*
 |  I2cStop()
 |
 |  Stop data transfer by driving the clock and data lines to the necessary 
 |  states signifying a stop condition
 |
 |  Entry:  None
 |  Exit:   None   
 *---------------------------------------------------------------------------*/
void I2cStop(void);

/*---------------------------------------------------------------------------*
 |  I2cRead()
 |
 |  Read a data byte from the I2C slave.  The ack bit can be optionally
 |  sent if block transfers are requested.
 |
 |  Entry:  doiack - Flag to signify if the acknowledge signal should be
 |                   sent follwing the read of the byte
 |  Exit:   Byte read is returned.
 *---------------------------------------------------------------------------*/
uint8_t I2cRead(uint8_t doiack);

/*---------------------------------------------------------------------------*
 |  I2cWrite()
 |
 |  Send a data byte to the I2C slave.  The ack bit is then checked to 
 |  verify the write was accepted.
 |
 |  Entry:  byte - Data byte to be sent to slave.
 |  Exit:   Flag indicating if write was successful.
 *---------------------------------------------------------------------------*/
uint8_t I2cWrite(uint8_t byte);

