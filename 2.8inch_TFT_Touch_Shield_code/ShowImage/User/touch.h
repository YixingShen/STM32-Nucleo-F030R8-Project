#ifndef __TOUCH_H_
#define __TOUCH_H_

#include "stm32f0xx.h"

//TP_CS: PB5
#define TP_CS_SET    GPIOB->ODR |= GPIO_ODR_5
#define TP_CS_CLR    GPIOB->ODR &= ~GPIO_ODR_5
//TP_IRQ: PB3
#define TP_IRQ_LOW    (GPIOB->IDR & GPIO_IDR_3)==0
#define TP_IRQ_HIGH   (GPIOB->IDR & GPIO_IDR_3)!=0


#define READ_TIMES  5
#define LOST_NUM    1

#define ERR_RANGE 50


typedef struct {
	uint16_t hwXpos0;
	uint16_t hwYpos0;
	uint16_t hwXpos;
	uint16_t hwYpos;
	uint8_t chStatus;
	uint8_t chType;
	short iXoff;
	short iYoff;
	float fXfac;
	float fYfac;
} tp_dev_t;


#define TP_PRESS_DOWN           0x80
#define TP_PRESSED              0x40


extern void xpt2046_init(void);
extern void tp_draw_touch_point(uint16_t hwXpos, uint16_t hwYpos, uint16_t hwColor);
extern void tp_draw_big_point(uint16_t hwXpos, uint16_t hwYpos, uint16_t hwColor);
extern void tp_show_info(uint16_t hwXpos0, uint16_t hwYpos0,
                         uint16_t hwXpos1, uint16_t hwYpos1,
                         uint16_t hwXpos2, uint16_t hwYpos2,
                         uint16_t hwXpos3, uint16_t hwYpos3, uint16_t hwFac);
extern uint8_t tp_scan(uint8_t chCoordType);
extern void tp_adjust(void);
extern void tp_dialog(void);
extern void tp_draw_board(void);

#endif
