#include "stm32f0xx.h"

#define Send_Mode
//#define Receive_Mode

//Address code
#define ADD_INI   0XAF89

#define	REG_RD	0x80
#define	REG_WR	0x7F
#define	RF_GAP	35					//30=3.5us, time constant between succesive RF register accesses
#define	gLength  20					//Max length packet
#define	period_remote 200

extern unsigned int address_code;
#ifdef Send_Mode
extern unsigned char go_sleep,fun_code,data_code,flag_RFsend;
#endif
#ifdef Receive_Mode
extern unsigned char fun_code_rec,data_code_rec;
extern unsigned int address_code_rec;
#endif
extern unsigned char DAT[gLength+1];
extern unsigned char tx[5],rx[5];
extern unsigned char send_size,receive_size;
   
void PL1167_Init(void);
void RF_EnterSleep(void);
void spi_write(unsigned char spi_bValue);
unsigned char spi_read(void);
#ifdef Send_Mode
void Send_DATA(unsigned char fun_code, unsigned char data_code);
#endif
#ifdef Receive_Mode
void Receive_DATA(void);
#endif

//PKT	input
#define PKT_IS_LOW   (GPIOA->IDR & GPIO_IDR_1)==0//PA1==0
#define PKT_IS_HIGHT (GPIOA->IDR & GPIO_IDR_1)!=0//PA1==1
//RFRST
#define RFRST_H    GPIOA->ODR |= GPIO_ODR_0//PA0=1
#define RFRST_L    GPIOA->ODR &= ~GPIO_ODR_0//PA0=0



#define LED2_ON     GPIOA->ODR |= GPIO_ODR_5
#define LED2_OFF    GPIOA->ODR &= ~GPIO_ODR_5
#define LED2_Toggle GPIOA->ODR ^= GPIO_ODR_5

//USER BUTTON
#define KEY_PRESS   (GPIOC->IDR & GPIO_IDR_13)==0
#define KEY_RELEASE (GPIOC->IDR & GPIO_IDR_13)!=0
