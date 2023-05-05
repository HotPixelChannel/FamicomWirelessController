#include "joypad.h"
#include "stm8s.h"
#include "hook.h"

u8 data = 0xFF;
u8 countEmpty = 0;
u8 turboState = 0;
u8 taFirstPressed, tbFirstPressed;

void joy_initBtns(void) {
  data = 0xFF;

  // Ð¡Ñ‚Ð°Ñ€Ñ‚ Ð±ÑƒÐ´ÐµÑ‚ Ñ Ð²Ð½ÐµÑˆÐ½Ð¸Ð¼ Ð¿Ñ€ÐµÑ€Ñ‹Ð²Ð°Ð½Ð¸ÐµÐ¼, Ñ‚Ð°Ðº Ð¶Ðµ Ð¾Ð½ Ð±ÑƒÐ´ÐµÑ‚ ÑÐ»ÑƒÐ¶Ð¸Ñ‚ÑŒ ÐºÐ½Ð¾Ð¿ÐºÐ¾Ð¹ Ð²ÐºÐ»ÑŽÑ‡ÐµÐ½Ð¸Ñ Ð´Ð¶Ð¾Ñ
  EXTI_SetExtIntSensitivity(EXTI_PORT_GPIOA, EXTI_SENSITIVITY_FALL_ONLY);
  GPIO_Init(JOY_START_PORT, (GPIO_Pin_TypeDef)(JOY_START_PIN), GPIO_MODE_IN_PU_IT);

  //Select
  GPIO_Init(JOY_SELECT_PORT, (GPIO_Pin_TypeDef)(JOY_SELECT_PIN), GPIO_MODE_IN_PU_NO_IT);

  //Start
  // GPIO_Init(JOY_START_PORT, (GPIO_Pin_TypeDef)(JOY_START_PIN), GPIO_MODE_IN_PU_NO_IT);

  //B
  GPIO_Init(JOY_B_PORT, (GPIO_Pin_TypeDef)(JOY_B_PIN), GPIO_MODE_IN_PU_NO_IT);
  //A
  GPIO_Init(JOY_A_PORT, (GPIO_Pin_TypeDef)(JOY_A_PIN), GPIO_MODE_IN_PU_NO_IT);

  //Turbo B
  GPIO_Init(JOY_B_TURBO_PORT, (GPIO_Pin_TypeDef)(JOY_B_TURBO_PIN), GPIO_MODE_IN_PU_NO_IT);
  //Turbo A
  GPIO_Init(JOY_A_TURBO_PORT, (GPIO_Pin_TypeDef)(JOY_A_TURBO_PIN), GPIO_MODE_IN_PU_NO_IT);

  // UP
  GPIO_Init(JOY_UP_PORT, (GPIO_Pin_TypeDef)(JOY_UP_PIN), GPIO_MODE_IN_PU_NO_IT);
  //Down
  GPIO_Init(JOY_DOWN_PORT, (GPIO_Pin_TypeDef)(JOY_DOWN_PIN), GPIO_MODE_IN_PU_NO_IT);
  //Left
  GPIO_Init(JOY_LEFT_PORT, (GPIO_Pin_TypeDef)(JOY_LEFT_PIN), GPIO_MODE_IN_PU_NO_IT);
  //Right
  GPIO_Init(JOY_RIGHT_PORT, (GPIO_Pin_TypeDef)(JOY_RIGHT_PIN), GPIO_MODE_IN_PU_NO_IT);
}

/******** Äëÿ ðàáîòû òóðáî êíîïîê **********/
void joy_toggleTurbo(void) {
  if (turboState > 0)
    turboState = 0;
  else
    turboState = 1;
}

/******** ÐžÐ¿Ñ€Ð¾Ñ ÐºÐ½Ð¾Ð¿Ð¾Ðº ************/
void joy_btnProc(void) {

  if (GPIO_ReadInputPin(JOY_A_TURBO_PORT, JOY_A_TURBO_PIN) == 0) {
    // Åñëè òîëüêî íàæàëè, ñðàçó ñòàâèì àêòèâíûé óðîâåíü
    if (taFirstPressed == 0) {
      turboState = 1;
      taFirstPressed = 1;
    }

    if (turboState > 0)
      data &= ~(1 << 7);
    else
      data |= (1 << 7);
  } else if (GPIO_ReadInputPin(JOY_A_PORT, JOY_A_PIN) == 0) {
    data &= ~(1 << 7);
  } else {
    taFirstPressed = 0;
    data |= (1 << 7);
  }

  if (GPIO_ReadInputPin(JOY_B_TURBO_PORT, JOY_B_TURBO_PIN) == 0) {

    // Åñëè òîëüêî íàæàëè, ñðàçó ñòàâèì àêòèâíûé óðîâåíü
    if (tbFirstPressed == 0) {
      turboState = 1;
      tbFirstPressed = 1;
    }

    if (turboState > 0)
      data &= ~(1 << 6);
    else
      data |= (1 << 6);
  } else if (GPIO_ReadInputPin(JOY_B_PORT, JOY_B_PIN) == 0) {
    data &= ~(1 << 6);
  } else {
    tbFirstPressed = 0;
    data |= (1 << 6);
  }

  if (GPIO_ReadInputPin(JOY_SELECT_PORT, JOY_SELECT_PIN) == 0) {
    data &= ~(1 << 5);
  } else {
    data |= (1 << 5);
  }

  if (GPIO_ReadInputPin(JOY_START_PORT, JOY_START_PIN) == 0) {
    data &= ~(1 << 4);
  } else {
    data |= (1 << 4);
  }

  if (GPIO_ReadInputPin(JOY_UP_PORT, JOY_UP_PIN) == 0) {
    data &= ~(1 << 3);
  } else {
    data |= (1 << 3);
  }

  if (GPIO_ReadInputPin(JOY_DOWN_PORT, JOY_DOWN_PIN) == 0) {
    data &= ~(1 << 2);
  } else {
    data |= (1 << 2);
  }

  if (GPIO_ReadInputPin(JOY_LEFT_PORT, JOY_LEFT_PIN) == 0) {
    data &= ~(1 << 1);
  } else {
    data |= (1 << 1);
  }

  if (GPIO_ReadInputPin(JOY_RIGHT_PORT, JOY_RIGHT_PIN) == 0) {
    data &= ~(1 << 0);
  } else {
    data |= (1 << 0);
  }

}

/******** Ð’Ñ‹Ð²Ð¾Ð´ Ð¼Ð°ÑÑÐ¸Ð²Ð° Ð´Ð°Ð½Ð½Ñ‹Ñ… **********/
u8 joy_getPayload() {
  if (data != 0xFF) {
    countEmpty = COUNT_OF_NULLS;
  }
  return data;
}

/******** Ð•ÑÐ»Ð¸ Ð½Ð°Ð¶Ð°Ñ‚Ð¸Ð¹ Ð½ÐµÑ‚ *************/
u8 joy_hasAction(void) {
  if (countEmpty > 0)
    countEmpty--;
  return countEmpty;
}

void nullData(void) {
  data = 0xFF;
}