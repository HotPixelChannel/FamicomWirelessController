#include "joypad.h"
#include "stm8s.h"
#include "hook.h"

u8 data = 0xFF;
u8 countEmpty = 0;
u8 turboState = 0;
u8 taFirstPressed, tbFirstPressed;

void joy_initBtns(void) {
  data = 0xFF;

  // Старт будет с внешним прерыванием, так же он будет служить кнопкой включения джоя
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

/******** ��� ������ ����� ������ **********/
void joy_toggleTurbo(void) {
  if (turboState > 0)
    turboState = 0;
  else
    turboState = 1;
}

/******** Опрос кнопок ************/
void joy_btnProc(void) {

  if (GPIO_ReadInputPin(JOY_A_TURBO_PORT, JOY_A_TURBO_PIN) == 0) {
    // ���� ������ ������, ����� ������ �������� �������
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

    // ���� ������ ������, ����� ������ �������� �������
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

/******** Вывод массива данных **********/
u8 joy_getPayload() {
  if (data != 0xFF) {
    countEmpty = COUNT_OF_NULLS;
  }
  return data;
}

/******** Если нажатий нет *************/
u8 joy_hasAction(void) {
  if (countEmpty > 0)
    countEmpty--;
  return countEmpty;
}

void nullData(void) {
  data = 0xFF;
}