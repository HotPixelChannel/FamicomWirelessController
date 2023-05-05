#include "stm8s.h"
#include "hook.h"


void Init_HW(void) {
  // LED
  //  GPIO_Init(GPIOB, GPIO_PIN_5,GPIO_MODE_OUT_PP_HIGH_FAST);

  // Установка ЦПУ на 16Мгц 
  CLK -> ECKR &= (uint8_t)(~CLK_ECKR_HSEEN);
  CLK -> CKDIVR &= (uint8_t)(~CLK_CKDIVR_HSIDIV); /* Clear High speed internal clock prescaler */
  CLK -> CKDIVR |= (uint8_t) CLK_PRESCALER_HSIDIV1; /* Set High speed internal clock prescaler */

  /*
  CLK_DeInit();
                
    CLK_HSECmd(DISABLE);
    CLK_LSICmd(DISABLE);
    CLK_HSICmd(ENABLE);
    while(CLK_GetFlagStatus(CLK_FLAG_HSIRDY) == FALSE);
                
    CLK_ClockSwitchCmd(ENABLE);
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
    CLK_SYSCLKConfig(CLK_PRESCALER_CPUDIV1);
                
    CLK_ClockSwitchConfig(CLK_SWITCHMODE_AUTO, CLK_SOURCE_HSI,  DISABLE, CLK_CURRENTCLOCKSTATE_ENABLE);
    */
  
  //Отрубаем все ненужное
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_I2C, DISABLE);
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_ADC, DISABLE);
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_AWU, DISABLE);
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_UART1, DISABLE);
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER1, DISABLE);
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER2, DISABLE);
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER4, ENABLE);

  // Конфиг внешних прерываний для порта D
  //  EXTI->CR1 &= (uint8_t)(~EXTI_CR1_PDIS);
  // EXTI->CR1 |= (uint8_t)((uint8_t)(EXTI_SENSITIVITY_FALL_ONLY) << 6);  

  // Конфиг TIM4
  TIM4_TimeBaseInit(TIM4_PRESCALER_16, (int)(1000000 * 0.08 - 1));
  TIM4_SetCounter(0x00);
  TIM4_ClearFlag(TIM4_FLAG_UPDATE);
  TIM4_ITConfig(TIM4_IT_UPDATE, ENABLE);
  TIM4_Cmd(ENABLE);
}

CONST uint8_t HSIDivFactors[4] = {1, 2, 4, 8}; /*!< Holds the different HSI Divider factors */

uint32_t GetClockFreq(void) {
  uint32_t clockfrequency = 0;
  CLK_Source_TypeDef clocksource = CLK_SOURCE_HSI;
  uint8_t tmp = 0, presc = 0;

  /* Get CLK source. */
  clocksource = (CLK_Source_TypeDef) CLK -> CMSR;

  if (clocksource == CLK_SOURCE_HSI) {
    tmp = (uint8_t)(CLK -> CKDIVR & CLK_CKDIVR_HSIDIV);
    tmp = (uint8_t)(tmp >> 3);
    presc = HSIDivFactors[tmp];
    clockfrequency = HSI_VALUE / presc;
  } else if (clocksource == CLK_SOURCE_LSI) {
    clockfrequency = LSI_VALUE;
  } else {
    clockfrequency = HSE_VALUE;
  }

  return ((uint32_t) clockfrequency);
}