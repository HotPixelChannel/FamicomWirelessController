#ifndef __HOOK_H
#define __HOOK_H

#include "stm8s.h"



void delay_us(void);
void delay_ms(unsigned int time);



void Init_HW(void);
void ledOn(void);
void ledOff(void);
void ledOffHard(void);

void watchCharge(u8 toOn);

void blinkT(u8 times, u8 delay);
uint32_t GetClockFreq(void);
void ledToggle(void);

uint8_t ADC_Read(void);
u8 isCharging(void);
void tick(void);


#endif

