#ifndef _JOYPAD_
#define _JOYPAD_

#include "stm8s.h"

#define COUNT_OF_NULLS  10     // Количество сообщений об отсутствии нажатий

#define JOY_B_TURBO_PORT   GPIOE   
#define JOY_B_TURBO_PIN    GPIO_PIN_5  

#define JOY_A_TURBO_PORT   GPIOC   
#define JOY_A_TURBO_PIN    GPIO_PIN_1

#define JOY_B_PORT   GPIOB   
#define JOY_B_PIN    GPIO_PIN_1  

#define JOY_A_PORT   GPIOB   
#define JOY_A_PIN    GPIO_PIN_0

#define JOY_START_PORT   GPIOA   
#define JOY_START_PIN    GPIO_PIN_3

#define JOY_SELECT_PORT   GPIOD   
#define JOY_SELECT_PIN    GPIO_PIN_7

#define JOY_UP_PORT   GPIOD   
#define JOY_UP_PIN    GPIO_PIN_3

#define JOY_DOWN_PORT   GPIOD   
#define JOY_DOWN_PIN    GPIO_PIN_4

#define JOY_LEFT_PORT   GPIOD   
#define JOY_LEFT_PIN    GPIO_PIN_2

#define JOY_RIGHT_PORT   GPIOD   
#define JOY_RIGHT_PIN    GPIO_PIN_5




/************ PROTOTYPES *************/
void joy_initBtns(void);
u8  joy_getPayload(void);
u8 joy_hasAction(void);
void joy_btnProc(void);
void joy_toggleTurbo(void);
void nullData(void);

#endif 