#ifndef pl1167_h
#define pl1167_h

#include "stm32f0xx.h"

//#define Send_Mode
#define Receive_Mode

extern unsigned int address_code;
#ifdef Send_Mode
extern unsigned char go_sleep,fun_code,data_code,flag_RFsend;
#endif
#ifdef Receive_Mode
extern unsigned char fun_code_rec,data_code_rec;
extern unsigned int address_code_rec;
#endif

#define	REG_RD	0x80
#define	REG_WR	0x7F
#define	RF_GAP	35					//30=3.5us, time constant between succesive RF register accesses
#define	gLength 20					//Max length packet
#define	period_remote 200

void pdelay (unsigned char t);
void delay_msec(unsigned int x);

void msec(unsigned int x);
void RF_init(void);
//void Set_RF_Channel(unsigned char channel);
void TX_packet(unsigned char *ptr,unsigned char bytes); //only tx loop
void RX_packet(void);
void PL1167_Init(void);
void RF_EnterSleep(void);
#ifdef Send_Mode
void Send_DATA(unsigned char fun_code, unsigned char data_code);
#endif
#ifdef Receive_Mode
void Receive_DATA(void);
#endif

void Reg_write16(unsigned char addr, unsigned char v1, unsigned char v2);
unsigned int Reg_read16(unsigned char addr);



//Address code
#define ADD_INI   0XAF89

//SPI for PL1167
#if 0
//SPICLK
#define SPICLK_H    GPIOB->ODR |= GPIO_ODR_0//PB0=1
#define SPICLK_L    GPIOB->ODR &= ~GPIO_ODR_0//PB0=0
//SPIMISO input
#define SPIMISO_IS_H    (GPIOB->IDR & GPIO_IDR_4)!=0//PB4==1
#define SPIMISO_IS_L    (GPIOB->IDR & GPIO_IDR_4)==0//PB4==0
//SPIMOSI
#define SPIMOSI_H    GPIOB->ODR |= GPIO_ODR_5//PB5=1
#define SPIMOSI_L    GPIOB->ODR &= ~GPIO_ODR_5//PB5=0
//SPICS
#define SPICS_H    GPIOA->ODR |= GPIO_ODR_4//PA4=1
#define SPICS_L    GPIOA->ODR &= ~GPIO_ODR_4//PA4=0
#else
//SPICLK
#define SPICLK_H    GPIOB->ODR |= GPIO_ODR_13//PB13=1
#define SPICLK_L    GPIOB->ODR &= ~GPIO_ODR_13//PB13=0
//SPIMISO input
#define SPIMISO_IS_H    (GPIOB->IDR & GPIO_IDR_14)!=0//PB14==1
#define SPIMISO_IS_L    (GPIOB->IDR & GPIO_IDR_14)==0//PB41==0
//SPIMOSI
#define SPIMOSI_H    GPIOB->ODR |= GPIO_ODR_15//PB15=1
#define SPIMOSI_L    GPIOB->ODR &= ~GPIO_ODR_15//PB15=0
//SPICS
#define SPICS_H    GPIOB->ODR |= GPIO_ODR_12//PB12=1
#define SPICS_L    GPIOB->ODR &= ~GPIO_ODR_12//PB12=0
#endif
//PKT	input
#define PKT_IS_LOW   (GPIOA->IDR & GPIO_IDR_1)==0//PA1==0
#define PKT_IS_HIGHT (GPIOA->IDR & GPIO_IDR_1)!=0//PA1==1
//RFRST
#define RFRST_H    GPIOA->ODR |= GPIO_ODR_0//PA0=1
#define RFRST_L    GPIOA->ODR &= ~GPIO_ODR_0//PA0=0


#define LED2_ON     GPIOA->ODR |= GPIO_ODR_5
#define LED2_OFF    GPIOA->ODR &= ~GPIO_ODR_5
#define LED2_FLASH  GPIOA->ODR ^= GPIO_ODR_5

//USER BUTTON
#define KEY_PRESS   (GPIOC->IDR & GPIO_IDR_13)==0
#define KEY_RELEASE (GPIOC->IDR & GPIO_IDR_13)!=0
#endif
