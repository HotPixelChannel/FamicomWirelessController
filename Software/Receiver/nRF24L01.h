// nRF24L01.h
#ifndef _NRF_24L01_
#define _NRF_24L01_
/*--------------------------------------------------------*/
#include "stm8s.h"

/*--------------------执行端引脚定义表--------------------*/


//CE引脚：NRF24L01使能端口
#define NRF24L01_CE_PORT   GPIOD   
#define NRF24L01_CE_PIN    GPIO_PIN_3  


//CSN引脚：MUC中的SPI的NSS已经为他用,故用PC4软件控制NRF24L01的CSN
#define NRF24L01_CS_PORT   GPIOC
#define NRF24L01_CS_PIN   GPIO_PIN_4

//以下为SPI引脚

#define NRF24L01_SCK_PORT  GPIOC
#define NRF24L01_SCK_PIN   GPIO_PIN_5 

#define NRF24L01_MOSI_PORT  GPIOC 
#define NRF24L01_MOSI_PIN   GPIO_PIN_6

#define NRF24L01_MISO_PORT  GPIOC
#define NRF24L01_MISO_PIN   GPIO_PIN_7

//IRQ引脚 中断引脚，在发送完成，接收到应答，接收数据 时候，都会发送跳变，
//可以接在MCU的外部中断中，来出发控制器进行读写操作
#define NRF24L01_IRQ_PORT  GPIOD
#define NRF24L01_IRQ_PIN   GPIO_PIN_2 
/*-----------------------------------------------------------*/

//#define TX_ADR_WIDTH    5   // 5 bytes TX address width   5字节的地址宽度
//#define RX_ADR_WIDTH    5   // 5 bytes RX address width   5字节的地址宽度
//#define TX_PLOAD_WIDTH  2 // 32 bytes TX payload         32字节的用户数据宽度
//#define RX_PLOAD_WIDTH  2  // 32 bytes TX payload        32字节的用户数据宽度

/*-----------------------------------------------------*/


#define RF_PA_MIN 0
#define RF_PA_LOW 1
#define RF_PA_MID 2
#define RF_PA_MAX 3


//****************************************************************//
// SPI(nRF24L01) commands
#define READ_REG_NRF        0x00  // Define read command to register  读配置寄存器,低5位为寄存器地址
#define WRITE_REG_NRF       0x20  // Define write command to register 写配置寄存器,低5位为寄存器地址
#define RD_RX_PLOAD     0x61  // Define RX payload register address  读RX有效数据,1~32字节
#define WR_TX_PLOAD     0xA0  // Define TX payload register address  写TX有效数据,1~32字节
#define FLUSH_TX        0xE1  // Define flush TX register command    清除TX FIFO寄存器命令
#define FLUSH_RX        0xE2  // Define flush RX register command    清除RX FIFO寄存器命令
#define REUSE_TX_PL     0xE3  // Define reuse TX payload register command  重新使用上一包数据,CE为高,数据包被不断发送
#define NOP             0xFF  // Define No Operation, might be used to read status register 空操作,可以用来读状态寄存器	


//***************************************************//
// SPI(nRF24L01) registers(addresses)
#define CONFIG          0x00  // 'Config' register address
                              //Адрес регистра конфигурации, бит 0: режим приема 1, режим передачи 0, бит 1: электрический выбор, бит 2: режим CRC, бит 3: включение CRC.
                             //бит 4: прерывание MAX_RT (максимальное время повторной передачи) разрешено; бит 5: разрешено прерывание TX_DS; бит 6: разрешено прерывание RX_DR
#define EN_AA           0x01  // 'Enable Auto Acknowledgment' register address  
                             //Включить функцию автоматического ответа бит0~5, соответствующую каналу 0~5
#define EN_RXADDR       0x02  // 'Enabled RX addresses' register address        
                              //Разрешен адрес приема, бит 0~5, соответствующий каналу 0~5
#define SETUP_AW        0x03  // 'Setup address width' register address         
                              //Установить ширину адреса (все каналы данных): бит1, 0:00, 3 байта, 01, 4 байта, 02, 5 байтов,
#define SETUP_RETR      0x04  // 'Setup Auto. Retrans' register address         
                              //Настройка автоматической повторной передачи, бит3:0, счетчик автоматической повторной передачи, бит7:4, задержка автоматической повторной передачи 250*x+86us
#define RF_CH           0x05  // 'RF channel' register address                  
                              //ВЧ-канал, бит 6:0, рабочая частота канала
#define RF_SETUP        0x06  // 'RF setup' register address                    
                              // Регистр RF, бит 3: скорость передачи (0: 1 Мбит/с, 1: 2 Мбит/с), бит 2: 1, мощность передачи, бит 0: усиление малошумящего усилителя
#define STATUS          0x07  // 'Status' register address
                              //Регистр состояния; бит 0: флаг заполнения TX FIFO; бит 3: 1, номер канала приема данных (максимум: 6); бит 4, достижение максимального количества повторных передач
                              //бит 5: завершение передачи данных прерывания, бит 6: прием данных прерывания;
#define OBSERVE_TX      0x08  // 'Observe TX' register address
                              // Регистр обнаружения отправки, бит 7:4, счетчик потери пакетов, бит 3:0, счетчик повторной передачи
#define CD              0x09  // 'Carrier Detect' register address
                             // Регистр обнаружения несущей, бит0, обнаружение несущей;
#define RX_ADDR_P0      0x0A  // 'RX address pipe0' register address
                              //Канал данных 0 получает адрес, максимальная длина 5 байт, младший байт идет первым
#define RX_ADDR_P1      0x0B  // 'RX address pipe1' register address
                              //Канал данных 1 получает адрес, максимальная длина 5 байт, младший байт идет первым
#define RX_ADDR_P2      0x0C  // 'RX address pipe2' register address
                              //Адрес приема канала данных 2, можно установить младший байт, старший байт должен быть равен RX_ADDR_P1[39:8];
#define RX_ADDR_P3      0x0D  // 'RX address pipe3' register address
                             //Канал данных 3 принимает адрес, можно установить младший байт, старший байт должен быть равен RX_ADDR_P1[39:8];
#define RX_ADDR_P4      0x0E  // 'RX address pipe4' register address
                              //Адрес приема канала данных 4, можно установить младший байт, старший байт должен быть равен RX_ADDR_P1[39:8];
#define RX_ADDR_P5      0x0F  // 'RX address pipe5' register address
                             //Канал данных 5 принимает адрес, можно установить младший байт, старший байт должен быть равен RX_ADDR_P1[39:8];
#define TX_ADDR         0x10  // 'TX address' register address
                              // Адрес отправки (сначала младший байт), в режиме ShockBurstTM RX_ADDR_P0 равен этому адресу
#define RX_PW_P0        0x11  // 'RX payload width, pipe0' register address
                             //Получите канал данных 0 эффективной ширины данных (1~32 байта), если он установлен в 0, это недопустимо
#define RX_PW_P1        0x12  // 'RX payload width, pipe1' register address
                              //Получить эффективную ширину данных канала 1 (1~32 байта), если установлено значение 0, это недопустимо
#define RX_PW_P2        0x13  // 'RX payload width, pipe2' register address
                             //Получите эффективную ширину данных канала 2 (1~32 байта), если она установлена ​​на 0, это недопустимо
#define RX_PW_P3        0x14  // 'RX payload width, pipe3' register address
                              //Получите эффективную ширину данных канала 3 (1~32 байта), если она установлена ​​на 0, это недопустимо
#define RX_PW_P4        0x15  // 'RX payload width, pipe4' register address
                              //Получите эффективную ширину данных канала 4 (1~32 байта), если она установлена ​​на 0, это недопустимо
#define RX_PW_P5        0x16  // 'RX payload width, pipe5' register address
                              //Получить эффективную ширину данных канала 5 (1~32 байта), если установлено значение 0, это недопустимо
#define FIFO_STATUS     0x17  // 'FIFO Status Register' register address
                              //Регистр состояния FIFO; бит0, флаг RX FIFO пустой; бит1, флаг заполнения RX FIFO; бит2, 3, зарезервировано
                              //бит4, флаг TX FIFO пустой, бит5, флаг заполнения TX FIFO, бит6, 1, циклически отправлять предыдущий пакет данных, 0, не циклически;

//************************Следующее добавлено *****************************//
#define MAX_TX  	0x10  // Прерывание при достижении максимального количества передач
#define TX_OK   	0x20  // Прерывание завершения передачи TX
#define RX_OK   	0x40  // прерывание приема данных

#define  t_tx           100   // максимальное количество повторных передач
#define  t_rx           100   // максимальное количество воспоминаний


#define RF_2MBPS 1
#define RF_1MBPS 0
//***************************************************************//
//                   FUNCTION's PROTOTYPES  //
/****************************************************************/
// void SPI_Init(u8 Mode);     // Init HW or SW SPI
 u8 SPI_RW(u8 byte);                                // 单字节SPI读写

 u8 NRF24L01_Read_Reg(u8 reg);                   //从nrf寄存器中读取一个字节  
 u8 NRF24L01_Write_Reg(u8 reg ,u8 value);                 // 写一个字节去nrf寄存器
 u8 NRF24L01_Write_Buf(u8 reg, u8 *pBuf, u8 bytes);  // записать несколько байтов в регистр
 u8 NRF24L01_Read_Buf(u8 reg, u8 *pBuf, u8 bytes);   // 从寄存器读取多字节
//*****************************************************************/

 
void inerDelay_us(unsigned int n);

void Init_NRF(void);
void NRF_RTinit(void);
void NRF24L01_TX_Mode(void);
void NRF24L01_RX_Mode(void);
void NRF24L01_RT_Mode(void);
u8 NRF24L01_RxPacket(u8 * rx_buf);
u8 NRF24L01_TxPacket(u8 * tx_buf);
u8 NRF24L01_Check(void);
u8 NRF_Get_State(void);
void clear_status(void);
void nrf_RxTx(u8 mod_nrf,u8 *buf);
void sendtorec(void);

void NRF24L01_setTxAddress(u8 *addr, u8 tx_addr_w);
void NRF24L01_setRxAddress(u8 *addr, u8 rx_addr_w);
void NRF24L01_setRate(u8 rate);
void NRF24L01_setPayloadSize(u8 size);
void NRF24L01_setPA(u8 pa);
void NRF24L01_setChannel(u8 cannel);
void NRF24L01_setAutoAck(u8 enable);

u8 NRF24L01_TxIsFull(void);
u8 NRF24L01_TxSent(void);
 void NRF24L01_setCpuFreq(u8 freq); //MHz


//extern uchar const TX_ADDRESS[TX_ADR_WIDTH];//TX address
//extern uchar const RX_ADDRESS[RX_ADR_WIDTH];//;RX address


#endif   //_NRF_24L01_
