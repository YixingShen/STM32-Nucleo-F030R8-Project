#ifndef __KEY_H
#define __KEY_H

#include "stm32f0xx.h"

#define KEY_NONE  0
#define KEY_USER  1

extern uint8_t key_id,key_id_done;

void KEY_GPIO_Init(void);
void KEY_Scan(void);

#endif
