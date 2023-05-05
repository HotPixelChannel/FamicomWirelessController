/**
	Dendy wireless controller (Transmitter)
	ver. 1.0
	(C) Roman V, 2023
	https://github.com/HotPixelChannel/FamicomWirelessController
**/

#include "stm8s.h"
#include "hook.h"
#include "nRF24L01.h"
#include "joypad.h"

#define TIME_OUT 180 // Время отключения, сек

u8 state = 1;
u8 stat = 0;
u8 TIM4_INT_counter;
u32 timeout = TIME_OUT;
u8 timCount;

u8 bufJ;
u8 clk;

// Address joy1
  u8 txAddr[] = {
    0x2A,
    0xA2,
    0x01,
    0xA2,
    0xA2
  };
  
  // Address joy2
  u8 txAddr2[] = {
  0x3b,
  0xb3,
  0x12,
  0xb3,
  0xb3
};

void delay(long cycles) {
  int k = 0;
  int i = 0;
  for (; i < cycles; i++) {
    k++;
  }
}

// индикация ошибки  
void error(void) {
  blinkT(10, 5);
}

void initRadio(void) {
  Init_NRF();
  delay(10);

  if (NRF24L01_Check() > 0) {
    error();
  }

  NRF24L01_setTxAddress(txAddr, 5);
  NRF24L01_setChannel(0x66);
  NRF24L01_setPayloadSize(1);
  NRF24L01_setRate(RF_1MBPS);
  NRF24L01_setPA(RF_PA_MID);
  NRF24L01_setAutoAck(0);
  NRF24L01_TX_Mode();
}

void main(void) {

  Init_HW();
  blinkT(1, 5);
  initRadio();
  joy_initBtns();

  clk = GetClockFreq() / 1000000;
  NRF24L01_setCpuFreq(clk);

  enableInterrupts();

  while (1) {

    bufJ = joy_getPayload(); // Опрос кнопок

    if (state == 0)
      ledOffHard();
    else if (state == 3) {
      ledOff();
    } else {
      joy_btnProc();
      ledOn();
    }

    switch (state) {
    case 0: { // Уходим в сон 	
      if (isCharging())
        state = 3;
      else
        halt();
      break;
    }

    case 1: // Ожидание
      if (bufJ != 0xFF | joy_hasAction() > 0) { // Если кнопка нажата или счетчик путых посылок больше 0			
        timeout = TIME_OUT;
        state = 2;
      }

      if (timeout == 0) { // Уходим в сон
        state = 0;
      }
      break;

    case 2: { // Передача данных
      if (bufJ != 0xFF | joy_hasAction() > 0) {
        stat = NRF24L01_TxPacket( & bufJ);
        while (GPIO_ReadInputPin(NRF24L01_IRQ_PORT, NRF24L01_IRQ_PIN) == 0); // Ждем окончания передачи
        if (stat != TX_OK) {
          state = 1;
          error();
        //  initRadio();
        }
      } else
        state = 1;
      break;
    }

    case 3: { // Режим зарядки (управление не работает)    
      ledOff();
      if (isCharging()) {
        blinkT(2, 110);
      } else {
        state = 0;
      }
      break;
    }

    default:
      state = 1;
    }
  }
}

INTERRUPT_HANDLER(TIM2_UPD_OVF_BRK_IRQHandler, 13) {
  TIM2_ClearFlag(TIM2_FLAG_UPDATE);
  joy_toggleTurbo();
}

INTERRUPT_HANDLER(TIM4_UPD_OVF_IRQHandler, 23) {
  TIM4_ClearFlag(TIM4_FLAG_UPDATE);
  timCount++;
  tick();

  //Индикация зарядки
  if (isCharging()) {
    blinkT(2, 50);
  }

  if (timCount == 60) {
    timCount = 0;
    timeout--;
  }
}

//Прерывание кнопки Старт
INTERRUPT_HANDLER(EXTI_PORTA_IRQHandler, 3) {

  if (state != 0 & state != 3)
    return;

  timeout = TIME_OUT;
  state = 1;
  initRadio();
  ledOn();

  while (GPIO_ReadInputPin(JOY_START_PORT, JOY_START_PIN) == 0);

}

// Прерывание пина зарядки
INTERRUPT_HANDLER(EXTI_PORTB_IRQHandler, 4) {
  if (state != 0)
    return;

  if (isCharging())
    state = 3;
}

#ifdef USE_FULL_ASSERT
void assert_failed(u8 * file, u32 line) {
  while (1) {}
}
#endif