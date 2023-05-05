#include "stm8s.h"
#include "hook.h"

#define LED_PORT GPIOB
#define LED_PIN GPIO_PIN_6

#define CHARGE_DET_PORT GPIOB
#define CHARGE_DET_PIN GPIO_PIN_7

u8 blTimes = 0;
u8 blDelay = 0;
u8 blState = 0;
uint32_t lastBlink = 0;
u8 lastStateLed = 0;

uint32_t current_millis = 0;

void ledOn(void) {
  lastStateLed = 1;
  // GPIO_WriteHigh(LED_PORT,LED_PIN);
}

void ledOff(void) {
  lastStateLed = 0;
  // GPIO_WriteLow(LED_PORT,LED_PIN);
}

void ledOffHard(void) {
  lastStateLed = 0;
  blTimes = 0;
  GPIO_WriteLow(LED_PORT, LED_PIN);
}

u8 isCharging(void) {
  if (GPIO_ReadInputPin(CHARGE_DET_PORT, CHARGE_DET_PIN) == 0)
    return 0;
  else
    return 1;
}

void blinkT(u8 times, u8 delay) {
  blTimes = times * 2;
  blDelay = delay;
}

void ledToggle(void) {
  if (lastStateLed = 0) {
    lastStateLed = 1;
    GPIO_WriteHigh(LED_PORT, LED_PIN);
  } else {
    lastStateLed = 0;
    GPIO_WriteLow(LED_PORT, LED_PIN);
  }
}

void tick(void) {
  current_millis++;

  //blinking
  if (blTimes > 0) {

    if (current_millis - lastBlink > blDelay) {
      if (blState == 0) {
        GPIO_WriteLow(LED_PORT, LED_PIN);
        blState = 1;
      } else {
        GPIO_WriteHigh(LED_PORT, LED_PIN);
        blState = 0;
      }

      blTimes--;
      lastBlink = current_millis;

    }
  } else {
    if (lastStateLed == 1)
      GPIO_WriteHigh(LED_PORT, LED_PIN);
    else
      GPIO_WriteLow(LED_PORT, LED_PIN);
  }

}

void Init_HW(void) {
  GPIO_Init(GPIOA, GPIO_PIN_2, GPIO_MODE_OUT_OD_LOW_SLOW);
  GPIO_Init(GPIOA, GPIO_PIN_3, GPIO_MODE_OUT_OD_LOW_SLOW);

  // Charging indicator
  EXTI_SetExtIntSensitivity(EXTI_PORT_GPIOB, EXTI_SENSITIVITY_RISE_FALL);
  GPIO_Init(CHARGE_DET_PORT, CHARGE_DET_PIN, GPIO_MODE_IN_FL_IT);

  // LED
  GPIO_Init(LED_PORT, LED_PIN, GPIO_MODE_OUT_PP_HIGH_FAST);

  // РЈСЃС‚Р°РЅРѕРІРєР° Р¦РџРЈ РЅР° 2РњРіС† 
  CLK -> ECKR &= (uint8_t)(~CLK_ECKR_HSEEN);
  CLK -> CKDIVR &= (uint8_t)(~CLK_CKDIVR_HSIDIV); /* Clear High speed internal clock prescaler */
  CLK -> CKDIVR |= (uint8_t) CLK_PRESCALER_HSIDIV8; /* Set High speed internal clock prescaler */

  CLK_PeripheralClockConfig(CLK_PERIPHERAL_I2C, DISABLE);
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_ADC, DISABLE);
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_AWU, DISABLE);
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_UART1, DISABLE);
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER1, DISABLE);
  // CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER2, DISABLE);

  // РљРѕРЅС„РёРі РІРЅРµС€РЅРёС… РїСЂРµСЂС‹РІР°РЅРёР№ РґР»СЏ РїРѕСЂС‚Р° D
  EXTI -> CR1 &= (uint8_t)(~EXTI_CR1_PDIS);
  EXTI -> CR1 |= (uint8_t)((uint8_t)(EXTI_SENSITIVITY_FALL_ONLY) << 6);

  // РљРѕРЅС„РёРі TIM4
  TIM4_TimeBaseInit(TIM4_PRESCALER_128, 255);
  TIM4_SetCounter(0x00);
  TIM4_ClearFlag(TIM4_FLAG_UPDATE);
  TIM4_ITConfig(TIM4_IT_UPDATE, ENABLE);
  TIM4_Cmd(ENABLE);

  // TIM2 для работы турбо кнопок
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER2, ENABLE);
  TIM2_DeInit();
  TIM2_TimeBaseInit(TIM2_PRESCALER_2, (GetClockFreq() / 2) * 85E-3 - 1); //~13Гц
  TIM2_ITConfig(TIM2_IT_UPDATE, ENABLE);
  TIM2_Cmd(ENABLE);
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