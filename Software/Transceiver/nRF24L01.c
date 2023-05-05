//#include <string.h>
//#include <intrinsics.h>
#include "stm8s.h"

#include "nRF24L01.h"
 //#include "stdio.h"

u8 rec_buf[33]; //Еще один бит используется в качестве терминатора, nrf может отправлять и получать до 32 байтов, а строка по умолчанию равна 0 в качестве терминатора.
u8 tra_buf[32];

u8 * tx_addr; // = {0xaa,0xaa,0xaa,0xaa,0xaa};
u8 * rx_addr; // = {0xaa,0xaa,0xaa,0xaa,0xab};
u8 addr_width = 3;

u8 cpuFreq = 16;

u8 txReg = 0x07; // Настройки передатчика (2 Мбс, макс мощность)
u8 ch = 0; // Канал радио
u8 st_aa = 0; //Автоответ
u8 st_ps = 1; //Размер послыки

void ceL(void) {
  GPIO_WriteLow(NRF24L01_CE_PORT, NRF24L01_CE_PIN);
}

void ceH(void) {
  GPIO_WriteHigh(NRF24L01_CE_PORT, NRF24L01_CE_PIN);
}

void csL(void) {
  GPIO_WriteLow(NRF24L01_CS_PORT, NRF24L01_CS_PIN);
}

void csH(void) {
  GPIO_WriteHigh(NRF24L01_CS_PORT, NRF24L01_CS_PIN);
}

void inerDelay_us(unsigned int n) //????1US
{
  for (; n > 0; n--) {
    asm("nop"); //?STM8??,16M??,_nop_() ??? 333ns
    asm("nop");
    asm("nop");
    asm("nop");
  }
}

void delay_us1(u8 us) {
  u8 n = (cpuFreq * (us) / 4);
  for (; n > 0; n--) {
    asm("nop");
  }
}

void * _memcpy(void * dst,
  const void * src, u8 cnt) {
  unsigned char * d = (unsigned char * ) dst;
  const unsigned char * s = (const unsigned char * ) src;

  while (cnt--)
    *
    d++ = * s++;

  return dst;
}

void Init_NRF(void) {
  //Инициализируем порт ввода-вывода беспроводного модуля
  //MISO чтение данных с плавающим вводом, без внешнего прерывания
  GPIO_Init(NRF24L01_CE_PORT, (GPIO_Pin_TypeDef)(NRF24L01_CE_PIN), GPIO_MODE_OUT_PP_HIGH_FAST); //ce 
  GPIO_Init(NRF24L01_CS_PORT, (GPIO_Pin_TypeDef)(NRF24L01_CS_PIN), GPIO_MODE_OUT_PP_HIGH_FAST); //cs
  GPIO_Init(NRF24L01_SCK_PORT, (GPIO_Pin_TypeDef)(NRF24L01_SCK_PIN), GPIO_MODE_OUT_PP_HIGH_FAST); //sck
  GPIO_Init(NRF24L01_MOSI_PORT, (GPIO_Pin_TypeDef)(NRF24L01_MOSI_PIN), GPIO_MODE_OUT_PP_HIGH_FAST); //MOSI
  GPIO_Init(NRF24L01_MISO_PORT, (GPIO_Pin_TypeDef)(NRF24L01_MISO_PIN), GPIO_MODE_IN_FL_NO_IT); //MISO
  GPIO_Init(NRF24L01_IRQ_PORT, (GPIO_Pin_TypeDef)(NRF24L01_IRQ_PIN), GPIO_MODE_IN_PU_NO_IT); //IRQ

  SPI_DeInit();
  /* Initialize SPI in Slave mode  */
  SPI_Init(SPI_FIRSTBIT_MSB, SPI_BAUDRATEPRESCALER_2, SPI_MODE_MASTER, SPI_CLOCKPOLARITY_LOW,
    SPI_CLOCKPHASE_1EDGE, SPI_DATADIRECTION_2LINES_FULLDUPLEX, SPI_NSS_SOFT, (uint8_t) 0x07);
  /* Enable the SPI*/
  SPI_Cmd(ENABLE);

  GPIO_WriteLow(NRF24L01_CE_PORT, NRF24L01_CE_PIN);
  GPIO_WriteHigh(NRF24L01_CS_PORT, NRF24L01_CS_PIN);

}

u8 SPI_RW(u8 byte) {

  u8 retry;
  /*!< Wait until a data is transmitted */
  while (SPI_GetFlagStatus(SPI_FLAG_TXE) == RESET) {
    retry++;
    if (retry > 200) return 0;
  }

  /*!< Send the byte */
  SPI_SendData(byte);
  retry = 0;

  /*!< Wait until a data is received */
  while (SPI_GetFlagStatus(SPI_FLAG_RXNE) == RESET) {
    retry++;
    if (retry > 200) return 0;

  }
  /*!< Get the received data */
  byte = SPI_ReceiveData();
  return (byte);
}

//Регистр записи SPI
//reg: указываем адрес регистра
//value: значение для записи
u8 NRF24L01_Write_Reg(u8 reg, u8 value) {
  u8 status;

  GPIO_WriteLow(NRF24L01_CS_PORT, NRF24L01_CS_PIN); // CSN low, init SPI transaction

  status = SPI_RW(reg); // select register	
  SPI_RW(value); // ..and write value to it..

  GPIO_WriteHigh(NRF24L01_CS_PORT, NRF24L01_CS_PIN); // CSN high again

  return (status); // return nRF24L01 status byte
}

//??SPI????
//reg:??????

u8 NRF24L01_Read_Reg(u8 reg) {
  u8 reg_val;

  GPIO_WriteLow(NRF24L01_CS_PORT, NRF24L01_CS_PIN); // CSN low, initialize SPI communication...

  SPI_RW(reg); // Select register to read from..
  reg_val = SPI_RW(0); // ..then read registervalue

  GPIO_WriteHigh(NRF24L01_CS_PORT, NRF24L01_CS_PIN); // CSN high, terminate SPI communication

  return (reg_val); // return register value
}

//??????????????
//reg:???(??)
//*pBuf:????
//len:????
//???,??????????? 

u8 NRF24L01_Read_Buf(u8 reg, u8 * pBuf, u8 bytes) {
  u8 status, byte_ctr;

  GPIO_WriteLow(NRF24L01_CS_PORT, NRF24L01_CS_PIN); // Set CSN low, init SPI tranaction

  status = SPI_RW(reg); // Select register to write to and read status byte

  for (byte_ctr = 0; byte_ctr < bytes; byte_ctr++)
    pBuf[byte_ctr] = SPI_RW(0);

  GPIO_WriteHigh(NRF24L01_CS_PORT, NRF24L01_CS_PIN);

  return (status); // return nRF24L01 status byte
}

//?????????????
//reg:???(??)
//*pBuf:????
//len:????
//???,???????????

u8 NRF24L01_Write_Buf(u8 reg, u8 * pBuf, u8 bytes) {
  u8 status, byte_ctr;

  GPIO_WriteLow(NRF24L01_CS_PORT, NRF24L01_CS_PIN);
  status = SPI_RW(reg);

  /*  if(status&0x10)//MAX_RT???????????????
  //PB_ODR ^= 0b00100000;			
  NRF24L01_Read_Reg(WRITE_REG+STATUS,0x70);//?????1?????IRQ_PIN????*/

  for (byte_ctr = 0; byte_ctr < bytes; byte_ctr++)
    SPI_RW( * pBuf++);

  GPIO_WriteHigh(NRF24L01_CS_PORT, NRF24L01_CS_PIN);

  return (status);
}

//Эта функция переводит NRF24L01 в режим RX
// Установите адрес RX, запишите ширину данных RX, выберите радиочастотный канал, скорость передачи и LNA HCURR
//Когда CE становится высоким, он переходит в режим RX и может получать данные

void NRF24L01_RX_Mode(void) {
  ceL();

  //NRF24L01_Write_Buf(WRITE_REG + TX_ADDR, (u8*)TX_ADDRESS, TX_ADR_WIDTH);    // Writes TX_Address to nRF24L01
  NRF24L01_Write_Buf(WRITE_REG_NRF + RX_ADDR_P0, (u8 * ) rx_addr, addr_width); // Use the same address on the RX device as the TX device	
  NRF24L01_Write_Reg(WRITE_REG_NRF + EN_AA, st_aa); //Включено автоматическое подтверждение ACK на канале 0
  NRF24L01_Write_Reg(WRITE_REG_NRF + EN_RXADDR, 0x01); //??????????0, 
  NRF24L01_Write_Reg(WRITE_REG_NRF + RF_CH, ch); //   ???????2.4GHZ,??????
  NRF24L01_Write_Reg(WRITE_REG_NRF + RX_PW_P0, st_ps); //????????,?????32??
  NRF24L01_Write_Reg(WRITE_REG_NRF + RX_PW_P1, 0);
  NRF24L01_Write_Reg(WRITE_REG_NRF + RX_PW_P2, 0);
  NRF24L01_Write_Reg(WRITE_REG_NRF + RX_PW_P3, 0);
  NRF24L01_Write_Reg(WRITE_REG_NRF + RX_PW_P4, 0);
  NRF24L01_Write_Reg(WRITE_REG_NRF + RX_PW_P5, 0);
  NRF24L01_Write_Reg(WRITE_REG_NRF + RF_SETUP, txReg); // Установите скорость передачи на 2 МГц, мощность передачи на максимальное значение 0 дБ, низкое усиление шума
  NRF24L01_Write_Reg(WRITE_REG_NRF + CONFIG, 0x0b); //power up  1: PRX  ???????????;PWR_UP,EN_CRC,16BIT_CRC,???? 

  if (addr_width == 3)
    NRF24L01_Write_Reg(WRITE_REG_NRF + SETUP_AW, 1);
  else if (addr_width == 4)
    NRF24L01_Write_Reg(WRITE_REG_NRF + SETUP_AW, 2);
  else
    NRF24L01_Write_Reg(WRITE_REG_NRF + SETUP_AW, 3);

  ceH();
  //inerDelay_us(130);

}

//Эта функция переводит NRF24L01 в режим TX
// Установите адрес TX, запишите ширину данных TX, установите адрес автоматического ответа RX, заполните данные передачи TX, выберите канал RF, скорость передачи и LNA HCURR
//PWR_UP, включение CRC
//Когда CE становится высоким, он переходит в режим TX и может принимать данные
//CE выше 10 мкс, затем начните отправку.

void NRF24L01_TX_Mode(void) {
  ceL();

  NRF24L01_Write_Buf(WRITE_REG_NRF + TX_ADDR, (u8 * ) tx_addr, addr_width);
  NRF24L01_Write_Buf(WRITE_REG_NRF + RX_ADDR_P0, (u8 * ) rx_addr, addr_width);
  NRF24L01_Write_Reg(WRITE_REG_NRF + EN_AA, st_aa);

  if (addr_width == 3)
    NRF24L01_Write_Reg(WRITE_REG_NRF + SETUP_AW, 1);
  else if (addr_width == 4)
    NRF24L01_Write_Reg(WRITE_REG_NRF + SETUP_AW, 2);
  else
    NRF24L01_Write_Reg(WRITE_REG_NRF + SETUP_AW, 3);

  //NRF24L01_Write_Reg(WRITE_REG_NRF + SETUP_RETR, 0x5f);
  NRF24L01_Write_Reg(WRITE_REG_NRF + EN_RXADDR, 0);
  NRF24L01_Write_Reg(WRITE_REG_NRF + RF_CH, ch);
  NRF24L01_Write_Reg(WRITE_REG_NRF + RF_SETUP, txReg);
  NRF24L01_Write_Reg(WRITE_REG_NRF + CONFIG, 0x0a);
  NRF24L01_Write_Reg(WRITE_REG_NRF + OBSERVE_TX, 0x0f);

  ceH();
}

/****Инициализация режима трансивера NRF24L01 ********/
/*void NRF24L01_RT_Mode(void)
{
  
  clear_status();
  
  GPIO_WriteLow(NRF24L01_CE_PORT,NRF24L01_CE_PIN);
  
  NRF24L01_Write_Buf(WRITE_REG_NRF+TX_ADDR,(u8*)tx_addr,addr_width);//?TX???? 	
  NRF24L01_Write_Buf(WRITE_REG_NRF+RX_ADDR_P0,(u8*)rx_addr,addr_width);//?RX????
  NRF24L01_Write_Reg(WRITE_REG_NRF+EN_AA,st_aa);     //????0?????    
  NRF24L01_Write_Reg(WRITE_REG_NRF+EN_RXADDR,0x01); //????0?????  
  NRF24L01_Write_Reg(WRITE_REG_NRF+RF_CH,ch);       //??RF???0,????2400+ RH_CH mhz ????
  NRF24L01_Write_Reg(WRITE_REG_NRF+RX_PW_P0, st_ps);//????0???????
  // NRF24L01_Write_Reg(WRITE_REG_NRF+RF_SETUP,0); 
  NRF24L01_Write_Reg(WRITE_REG_NRF+RF_SETUP,txReg);  //??TX????,0db??,2Mbps,???????   
  NRF24L01_Write_Reg(WRITE_REG_NRF+CONFIG,0x0f);    //???????????:????,1~16?CRC??,IRQ??????
  
  GPIO_WriteHigh(NRF24L01_CE_PORT,NRF24L01_CE_PIN);  
}
*/

//??NRF24L01??????
//rxbuf:???????
//???:0,????;1,??????

u8 NRF24L01_RxPacket(u8 * rx_buf) {
  u8 stat;

  stat = NRF24L01_Read_Reg(STATUS); // read register STATUS's value

  if (stat & RX_OK) // if receive data ready (RX_DR) interrupt
  {
    NRF24L01_Read_Buf(RD_RX_PLOAD, (u8 * ) rx_buf, st_ps); // read receive payload from RX_FIFO buffer
    //  NRF24L01_Write_Reg(FLUSH_RX,0xff);  //Очистить регистр RX FIFO    
    clear_status();
    return 1; //we have receive data
  }

  NRF24L01_Write_Reg(WRITE_REG_NRF + STATUS, stat); // clear RX_DR or TX_DS or MAX_RT interrupt flag

  return 0;
}

/******** Установка скорости передачи (RF_1MBPS/RF_2MBPS)  ***********/
void NRF24L01_setRate(u8 rate) {
  if (rate)
    txReg |= (1 << 3);
  else
    txReg &= ~(1 << 3);
}

/******** Установка мощности передачи (RF_PA_MIN/RF_PA_LOW/RF_PA_MID/RF_PA_MAX) *******/
void NRF24L01_setPA(u8 pa) {
  if (pa > 3)
    pa = 3;
  txReg &= ~((1 << 2) | (1 << 1));
  txReg |= (pa << 1);
}

/**************************************************
Function: nRF24L01_TxPacket();

Description:
This function initializes one nRF24L01 device to
TX mode, set TX address, set RX address for auto.ack,
fill TX payload, select RF channel, datarate & TX pwr.
PWR_UP is set, CRC(2 bytes) is enabled, & PRIM:TX.

ToDo: One high pulse(>10us) on CE will now send this
packet and expext an acknowledgment from the RX device.

//Эта функция переводит NRF24L01 в режим TX
// Установите адрес TX, запишите ширину данных TX, установите адрес автоматического ответа RX, заполните данные передачи TX, выберите канал RF, скорость передачи и LNA HCURR
//PWR_UP, включение CRC
//Когда CE становится высоким, он переходит в режим TX и может принимать данные
//CE выше 10 мкс, затем начните отправку.


**************************************************/
u8 NRF24L01_TxPacket(u8 * tx_buf) {
  u8 stat;

  // GPIO_WriteLow(NRF24L01_CE_PORT,NRF24L01_CE_PIN);
  //  NRF24L01_Write_Buf(WR_TX_PLOAD, tx_buf, TX_PLOAD_WIDTH);     
  //  GPIO_WriteHigh(NRF24L01_CE_PORT,NRF24L01_CE_PIN);
  


  NRF24L01_Write_Buf(WR_TX_PLOAD, tx_buf, st_ps);
  ceH();
  csH();
  delay_us1(50);
  ceL();
  delay_us1(130);
  


 // while (GPIO_ReadInputPin(NRF24L01_IRQ_PORT, NRF24L01_IRQ_PIN) != 0); //ждать прерывания отправки
  if(NRF24L01_TxIsFull()){      // если стек передачи полон, даем немного времени раскидаться
   delay_us1(230); // NRF24L01_Write_Reg(FLUSH_TX, 0xff);
  }
  
  stat = NRF24L01_Read_Reg(STATUS); //Прочитать значение регистра состояния	   
  NRF24L01_Write_Reg(WRITE_REG_NRF + STATUS, stat); //Очистить флаг прерывания TX_DS или MAX_RT
  ceH();

  if (stat & MAX_TX) //Достигнуто максимальное количество повторных передач
  {
    NRF24L01_Write_Reg(FLUSH_TX, 0xff); //Очистить регистр TX FIFO
    return MAX_TX;
  }
  if (stat & TX_OK) //отправка завершена
  {
    return TX_OK;
  }
  return 0xff; //Не удалось отправить по другим причинам
}

/**************************************************/

// Включите питание, чтобы проверить, на месте ли NRF24L01
//Записываем 5 кусков данных и потом читаем обратно для сравнения, если совпадают, возвращаем значение: 0, значит на месте, иначе возвращаем 1
u8 NRF24L01_Check(void) {
  u8 buf[5] = {
    0xa9,
    0xa9,
    0xa9,
    0xa9,
    0xa9
  };
  u8 i;

  NRF24L01_Write_Buf(WRITE_REG_NRF + TX_ADDR, buf, 5); //Пишем 5 байт в адрес.	
  NRF24L01_Read_Buf(TX_ADDR, buf, 5); //Читаем адрес

  for (i = 0; i < 5; i++)
    if (buf[i] != 0xA9)
      break;
  if (i != 5)
    return 1; // проблемы с модулем 
  return 0; // все ОК
}

/******nrf24l01????*************/

u8 sta; //Хранение регистра состояния

void nrf_RxTx(u8 mod_nrf, u8 * buf) {
  static u8 mod_nrf_b; //static адрес не разглашается

  /****Войти в режим запуска****/

  if (mod_nrf == 't') {
    if (mod_nrf_b != 't') {
      mod_nrf_b = 't';
      GPIO_WriteLow(NRF24L01_CE_PORT, NRF24L01_CE_PIN);
      NRF24L01_Write_Reg(WRITE_REG_NRF + STATUS, NOP); // очистить флаг прерывания
      NRF24L01_Write_Reg(FLUSH_TX, NOP); //Очистить регистр TX_FIFO
      NRF24L01_Write_Reg(WRITE_REG_NRF + CONFIG, 0x7e); //IRQ Пин не отображает прерывание Режим передачи при включении питания Проверка 1~16CRC
      GPIO_WriteHigh(NRF24L01_CE_PORT, NRF24L01_CE_PIN);
      //nrf_delay_us(130);//От CE = 0 до CE = 1, то есть из режима ожидания в режим передачи и приема требуется максимум 130us		   
    }

    //******отправить данные******
    GPIO_WriteLow(NRF24L01_CE_PORT, NRF24L01_CE_PIN); //StandBy  режим
    NRF24L01_Write_Buf(WRITE_REG_NRF + RX_ADDR_P0, (u8 * ) rx_addr, addr_width); // Загрузить адрес получателя
    NRF24L01_Write_Buf(WR_TX_PLOAD, buf, st_ps); // загрузить данные
    ceH(); //Установите высокий CE, чтобы стимулировать передачу данных

    inerDelay_us(130); // От CE = 0 до CE = 1, то есть из режима ожидания в режим приемопередатчика, требуется максимум 130 мкс	
    inerDelay_us(100); // Дайте немного времени для отправки данных Быстрее, чем отправка Задержка может быть меньше, чем получение
    inerDelay_us(10);
    sta = NRF24L01_Read_Reg(STATUS); //?????????
    NRF24L01_Write_Reg(WRITE_REG_NRF + STATUS, sta); //???????

    if (sta & TX_OK) //???????tx fifo???  TX_DS??1
    {
      GPIO_WriteLow(NRF24L01_CE_PORT, NRF24L01_CE_PIN);
      NRF24L01_Write_Reg(FLUSH_TX, NOP); //??tx fifo???	//********??*********
      GPIO_WriteHigh(NRF24L01_CE_PORT, NRF24L01_CE_PIN);
    }

  }
  //******??????******
  else if (mod_nrf == 'r') //????
  {
    if (mod_nrf_b != 'r') {
      mod_nrf_b = 'r';
      GPIO_WriteLow(NRF24L01_CE_PORT, NRF24L01_CE_PIN);
      NRF24L01_Write_Reg(WRITE_REG_NRF + STATUS, NOP); //??????
      NRF24L01_Write_Reg(FLUSH_RX, NOP); //??RX_FIFO???
      NRF24L01_Write_Reg(WRITE_REG_NRF + CONFIG, 0x0f); //IRQ???? RX_RD?? ?? ????   1~16CRC??   
      GPIO_WriteHigh(NRF24L01_CE_PORT, NRF24L01_CE_PIN);
      inerDelay_us(130); //?CE = 0 ? CE = 1;??????????,????130us
    }
    inerDelay_us(500); //??? ???  ?????????

    sta = NRF24L01_Read_Reg(STATUS);
    NRF24L01_Write_Reg(WRITE_REG_NRF + STATUS, sta);

    if (sta & RX_OK) //RX_DR ?? ????
    {
      GPIO_WriteLow(NRF24L01_CE_PORT, NRF24L01_CE_PIN);
      NRF24L01_Read_Buf(RD_RX_PLOAD, buf, st_ps); //???? ????
      NRF24L01_Write_Reg(FLUSH_RX, NOP); //??rx fifo???	????? 
      GPIO_WriteHigh(NRF24L01_CE_PORT, NRF24L01_CE_PIN);
    }

  }

}

void clear_status(void) {

  u8 status;
  status = NRF24L01_Read_Reg(STATUS);
  NRF24L01_Write_Reg(WRITE_REG_NRF + STATUS, status);
  NRF24L01_Write_Reg(FLUSH_RX, NOP); // ??RX FIFO??? 
  NRF24L01_Write_Reg(FLUSH_TX, NOP); /* ??TX FIFO??? */

}

u8 NRF_Get_State(void) {
  u8 status;
  u8 rf_rec_flag;
  status = NRF24L01_Read_Reg(STATUS);
  NRF24L01_Write_Reg(WRITE_REG_NRF + STATUS, status); /*??TX_DS?MAX_RT?RX_DR????*/
  if (status & RX_OK) {
    NRF24L01_Read_Buf(RD_RX_PLOAD, rec_buf, st_ps); //????
    NRF24L01_Write_Reg(FLUSH_RX, NOP); // ??RX FIFO??? 
    //printf("%s",rec_buf);
    _memcpy(rec_buf, 0, 32);
    rf_rec_flag = RX_OK;
  } else if (status & MAX_TX) /* ???????? */ {
    NRF24L01_Write_Reg(FLUSH_TX, NOP); /* ??TX FIFO??? */
    rf_rec_flag = MAX_TX;
  } else if (status & TX_OK) /* ???? */ {
    NRF24L01_Write_Reg(FLUSH_TX, NOP); /* ??TX FIFO??? */
    rf_rec_flag = TX_OK;
  } else
    rf_rec_flag = 0; /* ??????? */

  return rf_rec_flag;
}

extern u8 setflag;
extern u8 cnt;

void NRF24L01_setTxAddress(u8 * addr, u8 tx_addr_w) {
  tx_addr = addr;
  addr_width = tx_addr_w;
}

void NRF24L01_setRxAddress(u8 * addr, u8 rx_addr_w) {
  rx_addr = addr;
  addr_width = rx_addr_w;
}

/*****  Установка канала ******/

void NRF24L01_setChannel(u8 channel) {
  ch = channel;
  NRF24L01_Write_Reg(WRITE_REG_NRF + RF_CH, ch);
}

/*****  Установка автоответа ******/
void NRF24L01_setAutoAck(u8 enable) {
  st_aa = enable;
}

void NRF24L01_setPayloadSize(u8 size) {
  st_ps = size;
}

/*****  Проверка заполненности очереди передачи ******/
u8 NRF24L01_TxIsFull(void) {
  u8 status;
  status = NRF24L01_Read_Reg(STATUS);

  return status & (1 << 0);
}

/*****  Сообщение отправлено ******/
u8 NRF24L01_TxIsSent(void) {
  u8 status;
  status = NRF24L01_Read_Reg(STATUS);

  return status & (1 << 5);
}

/******* Установка частоты процессора для рассчета delay *******/
void NRF24L01_setCpuFreq(u8 freq) {
  cpuFreq = freq;
}