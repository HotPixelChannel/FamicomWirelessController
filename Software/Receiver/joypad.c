#include "joypad.h"
#include "stm8s.h"

u8 data = 0xFF;
u8 stateJ = 0;

void joy_setData(u8 * dataI) {
  data = * dataI;
}

void joy_nullData(void) {
  data = 0xFF;
}

void joy_initBtns(void) {

  // JOY 1
  GPIO_Init(D0_PORT, (GPIO_Pin_TypeDef)(D0_PIN), GPIO_MODE_OUT_PP_HIGH_FAST);
  GPIO_Init(D1_PORT, (GPIO_Pin_TypeDef)(D1_PIN), GPIO_MODE_OUT_OD_HIZ_SLOW);
  GPIO_Init(D2_PORT, (GPIO_Pin_TypeDef)(D2_PIN), GPIO_MODE_OUT_OD_HIZ_SLOW);
  GPIO_Init(D3_PORT, (GPIO_Pin_TypeDef)(D3_PIN), GPIO_MODE_OUT_PP_HIGH_FAST);
  GPIO_Init(D4_PORT, (GPIO_Pin_TypeDef)(D4_PIN), GPIO_MODE_OUT_PP_HIGH_FAST);
  GPIO_Init(D5_PORT, (GPIO_Pin_TypeDef)(D5_PIN), GPIO_MODE_OUT_PP_HIGH_FAST);
  GPIO_Init(D6_PORT, (GPIO_Pin_TypeDef)(D6_PIN), GPIO_MODE_OUT_PP_HIGH_FAST);
  GPIO_Init(D7_PORT, (GPIO_Pin_TypeDef)(D7_PIN), GPIO_MODE_OUT_PP_HIGH_FAST);

  data = 0xFF;
}

void joy_pressBtns(void) {
  /* if(data!=0xff){
     data = data;
   }*/

  for (u8 i = 0; i < 8; i++) {
    switch (i) {
    case 0:
      if (data & (1 << i))
        D0_PORT -> ODR |= D0_PIN;
      else
        D0_PORT -> ODR &= (~D0_PIN);
      break;

    case 1:
      if (data & (1 << i))
        GPIO_WriteHigh(D1_PORT, D1_PIN);
      else
        GPIO_WriteLow(D1_PORT, D1_PIN);
      break;

    case 2:
      if (data & (1 << i))
        GPIO_WriteHigh(D2_PORT, D2_PIN);
      //D2_PORT->ODR |= D2_PIN;
      else
        //D2_PORT->ODR &= (~D2_PIN);  
        GPIO_WriteLow(D2_PORT, D2_PIN);
      break;

    case 3:
      if (data & (1 << i))
        D3_PORT -> ODR |= D3_PIN;
      else
        D3_PORT -> ODR &= (~D3_PIN);
      break;

    case 4:
      if (data & (1 << i))
        D4_PORT -> ODR |= D4_PIN;
      else
        D4_PORT -> ODR &= (~D4_PIN);
      break;

    case 5:
      if (data & (1 << i))
        D5_PORT -> ODR |= D5_PIN;
      else
        D5_PORT -> ODR &= (~D5_PIN);
      break;

    case 6:
      if (data & (1 << i))
        D6_PORT -> ODR |= D6_PIN;
      else
        D6_PORT -> ODR &= (~D6_PIN);
      break;

    case 7:
      if (data & (1 << i))
        D7_PORT -> ODR |= D7_PIN;
      else
        D7_PORT -> ODR &= (~D7_PIN);
      break;
    }
  }
}