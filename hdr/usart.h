#ifndef USART_H_INCLUDED
#define USART_H_INCLUDED

#include "stm32f10x.h"
#include "dma.h"

#define BAUDRATE_72_9600 0x00001D4C    //exact
#define BAUDRATE_72_115200 0x271       //exact
#define BAUDRATE_72_230400 0x138
#define BAUDRATE_72_921600 0x4E        //exact

#define USART1_IRQ_ON 1
#define USART1_IRQ_OFF 0
#define USART1_DMA_ON 1
#define USART1_DMA_OFF 0


#define NO_REMAP_USART1_RX_TX_PORTS 0
#define REMAP_USART1_RX_TX_PORTS 1

#define USART1_CTS_RTS_DISABLED 0  // *** ERRATA*** in this case we need to remap off CAN 
#define USART1_CTS_RTS_ENABLED 1   // *** ERRATA*** in this case we need to remap off CAN 


	
void Usart1PinConfig(uint32_t remapConfig, uint32_t ctsrtsConfig); 					// Configure pins used for USART1
void Usart1Config(uint32_t isIrqOn, uint32_t isDmaOn, uint32_t baudRate);		// USART1 configuration
void Usart1Start(uint32_t isIrqOn);																					// USART1 start
void Dma1Usart1_SendByte(uint8_t *pdata);																		// Send a single byte via DMA1 and USART1
void Dma1Usart1_SendByteArray(uint8_t *pdata, uint32_t ArraySize);					// Send a multi-byte array via DMA1 and USART1

#endif /* USART_H_INCLUDED*/
