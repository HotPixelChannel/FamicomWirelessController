#ifndef _JOYPAD_
#define _JOYPAD_

#include "stm8s.h"


#define D0_PORT   GPIOC   
#define D0_PIN    GPIO_PIN_3  
        
#define D1_PORT   GPIOB   
#define D1_PIN    GPIO_PIN_4
        
#define D2_PORT   GPIOB   
#define D2_PIN    GPIO_PIN_5  
        
#define D3_PORT   GPIOA   
#define D3_PIN    GPIO_PIN_3  
        
#define D4_PORT   GPIOA   
#define D4_PIN    GPIO_PIN_2  
        
#define D5_PORT   GPIOA   
#define D5_PIN    GPIO_PIN_1  
        
#define D6_PORT   GPIOD   
#define D6_PIN    GPIO_PIN_6  
        
#define D7_PORT   GPIOD  
#define D7_PIN    GPIO_PIN_5  





/************ PROTOTYPES *************/
void joy_initBtns(void);
void joy_pressBtns(void);
void joy_nullData(void);
void joy_setData(u8 * data);

#endif 