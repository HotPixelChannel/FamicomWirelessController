/**
    Dendy wireless controller (Receiver)
    ver. 1.0
    (C) Roman V, 2023
    https://github.com/HotPixelChannel/FamicomWirelessController
**/

#include "stm8s.h"
#include "hook.h"
#include "nRF24L01.h"
#include "joypad.h"

u8 nullByte = 0xFF;

u8 state = 0;
u16 TIM4_INT_counter;
u8 timCount;

u8 bufJ = 0xFF;
u8 clk;

void delay(long cycles) {
  int k = 0;
  for (int i = 0; i < cycles; i++) {
    k++;
  }
}

void initRadio() {
  Init_NRF();

  while (NRF24L01_Check() > 0) {
    // error();
  }

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

  NRF24L01_setRxAddress(txAddr, 5);
  NRF24L01_setChannel(0x66);
  NRF24L01_setPayloadSize(1);
  NRF24L01_setRate(RF_1MBPS);
  NRF24L01_setPA(RF_PA_MIN);
  NRF24L01_setAutoAck(0);
  NRF24L01_RX_Mode();
}

void main(void) {

  Init_HW();
  joy_initBtns();
  joy_nullData();
  joy_pressBtns();
  initRadio();
  

  clk = GetClockFreq() / 1000000;
  NRF24L01_setCpuFreq(clk);

  enableInterrupts();

  while (1) {

    if (NRF24L01_RxPacket( & bufJ) == 1) {
      TIM4_INT_counter = 0;
      state = 1;
    }

    switch (state) {
      case 0: {
        joy_nullData();
        break;
      }

      case 1: {
        joy_setData( &bufJ);
        break;
      }
    }

    joy_pressBtns();
  }
}

INTERRUPT_HANDLER(TIM4_UPD_OVF_IRQHandler, 23) {
  TIM4_ClearFlag(TIM4_FLAG_UPDATE);

  TIM4_INT_counter++;
  // Защита от "дребезга" в случае кратковременной потери связи
  if (TIM4_INT_counter > 1200 & bufJ!=nullByte) {
    TIM4_INT_counter = 0;
    state = 0;
  }
}

#ifdef USE_FULL_ASSERT
void assert_failed(u8 * file, u32 line) {
  while (1) {}
}
#endif