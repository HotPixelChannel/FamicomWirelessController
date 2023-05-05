#ifndef __HOOK_H
#define __HOOK_H

#include "stm8s.h"



void delay_us(void);
void delay_ms(unsigned int time);



void Init_HW(void);
void ledOn(void);
void ledOff(void);
uint32_t GetClockFreq(void);




#endif

