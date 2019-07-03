//*****************************************************************
// 
// 
// bxCAN controller functions
// todo: Add Extended id support, RTR
// Alex Svetlichnyy 2019
//*****************************************************************
#ifndef CAN_H_INCLUDED
#define CAN_H_INCLUDED

#include "stm32f10x.h"
#include "string_convertor.h"
#include "usart.h"
#include "device_init.h"

#define NO_REMAP_CAN1								0
#define REMAP_CAN1									1
#define CAN_INIT_TIMEOUT_MAX	    	0xfff
#define CAN_BAUDRATE_500KB    	  	500

#define CAN_TX_MAILBOX_0 					0
#define CAN_TX_MAILBOX_1 					1	
#define CAN_TX_MAILBOX_2 					2	

#define CAN_EMPTY_MAILBOX_TIMEOUT 0xfff
#define ERR_CAN1_NO_EMPTY_MAILBOX 4

#define CAN_FIFO_0 0	
#define CAN_FIFO_1 1


// Structure of a Standart (11-bit id) CAN message 
typedef struct Std_Can_Message{	
	uint8_t id_highbyte;	// First number of an id 					(BXX)
	uint8_t id_lowbyte; 	// Second, third numbers of an id (XBB)
	uint8_t dlc;					// Message size, number of bytes
	uint8_t data[8];			// Data
} Std_Can_Message_Type; //creating new type

uint32_t Can1_Initializate_Pins(uint32_t remapConfig); 									// Configure CAN1 pins
uint32_t Can1_Configure_BaudRate(uint32_t baudRate);										// Configure CAN1 baud rate
uint32_t Can1_Initializate(uint32_t remapConfig, uint32_t baudRate);		// General CAN1 initialization		
void Can1_InitializateFilters(void);																		// Initialize CAN1 filters

uint32_t Can1_GetEmptyMailbox(void);																							// CAN1:Get an empty transmit mailbox
uint32_t Can1_SetTxMessage(Std_Can_Message_Type canMessage, uint32_t mailboxNo);	// CAN1:Prepare a CAN transmission act with a message and mailbox number 
uint32_t Can1_Transmit_StdMessage(Std_Can_Message_Type canMessage);								// CAN1:Transmit a CAN message
Std_Can_Message_Type Can1_Recieve_StdMessage(uint32_t FifoNo);										// CAN1:Recieve a standart id Message
uint32_t Can1_Check_Fifo0Pending_StdMessage(void);																// CAN1:Check for any messages in FIFO0
void FlushCanTxBuffers(void);																											// CAN1:Reset TX mailboxes
	
#endif /* CAN_H_INCLUDED*/
