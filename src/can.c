//*****************************************************************
// 
// 
// bxCAN controller functions
// todo: Add Extended id support, RTR
// Alex Svetlichnyy 2019
//*****************************************************************

#include "can.h"

/**
* brief  Can1_Initializate_Pins : Initializes pins for CAN1 controller        
* param  Remap configuration
* param  
* retval 0 - OK  1 - Err
*/

uint32_t Can1_Initializate_Pins(uint32_t remapConfig){
	if(remapConfig==REMAP_CAN1){
		//todo: add pin initialization if remapped
	}
	else if(remapConfig==NO_REMAP_CAN1){
		GPIOA->CRH	&= ~GPIO_CRH_CNF11;	//  PA11--RX  PA12--TX
		GPIOA->CRH 	&= ~GPIO_CRH_MODE11;	 		
		GPIOA->CRH	&= ~GPIO_CRH_CNF12;	 		
		GPIOA->CRH	|= GPIO_CRH_CNF11_0;	 
		GPIOA->CRH	|= GPIO_CRH_CNF12_1;	 
		GPIOA->CRH 	|= GPIO_CRH_MODE12;	  		
	}			
	return 0;
}

/**
* brief  Can1_Configure_BaudRate : COnfiguring BaudRate for CAN1 controller        
* param  BaudRate configuration
* param  
* retval 0 - OK  1 - Err
*/

uint32_t Can1_Configure_BaudRate(uint32_t baudRate){	
	if(baudRate==CAN_BAUDRATE_500KB){// configuring baudRate
		CAN1->BTR &= ~CAN_BTR_TS1; // reset value
		CAN1->BTR &= ~CAN_BTR_TS2; // reset value
		CAN1->BTR &= ~CAN_BTR_SJW; // reset value

		CAN1->BTR |= 0x0007;       //Prescaler	
		CAN1->BTR |= (0x0A << 16); //BS1
		CAN1->BTR |= (0x05 << 20); //BS2
		return 0;	
	}
	//todo: add some more baudRate configurations
	else return 1;
}

/**
* brief  Can1_Initializate : Initializes registers for CAN1 controller and starts controller        
* param  remapConfig, baudRate
* param  
* retval 0 - OK  1 - Err
*/
uint32_t Can1_Initializate(uint32_t remapConfig, uint32_t baudRate){
	uint32_t timeout;											// Timeout counter	
	Can1_Initializate_Pins(remapConfig);	// Configure pins	
	RCC->APB1ENR |= RCC_APB1ENR_CAN1EN;		// Enable peripheral clock
	CAN1->MCR |= CAN_MCR_INRQ;   					// Entering Initialization mode
	CAN1->MCR &= ~CAN_MCR_SLEEP;   				// Quitting Sleep mode
	
	timeout=0;
	while(!(CAN1->MSR & CAN_MSR_INAK) && (timeout < CAN_INIT_TIMEOUT_MAX)){
		timeout++;	// Waiting for a flag CAN_MSR_INAK (This means, Initialization mode started)			
	}; 
	if(!(CAN1->MSR & CAN_MSR_INAK)){ // If a timeout passed away and CAN_MSR_INAK had not come up - Error
		return 1;
	}	
	Can1_Configure_BaudRate(baudRate);	//Configuring BaudRate
	CAN1->IER |= CAN_IER_FMPIE0; 				// Enable FIFO-0 Pending interrupt
	CAN1->MCR |= CAN_MCR_ABOM;					// Enable auto recovery out from Buss-off	
	
	CAN1->MCR &= ~CAN_MCR_INRQ;    			// Quit Initialization mode
	
	timeout=0;
	while((CAN1->MSR & CAN_MSR_INAK) && (timeout < CAN_INIT_TIMEOUT_MAX)){
		timeout++;			
	}; // Waiting for both CAN_MSR_SLAK and CAN_MSR_INAK flags down(This means, Normal mode started) 
	
	if((!(CAN1->MSR & CAN_MSR_INAK)) && (!(CAN1->MSR & CAN_MSR_SLAK))){ 
	//Check SLAK and INAK flags. If they are reset, Controller has entered normal mode
		return 0;
	}
	else return 1;		
}

/**
* brief  Can1_InitializateFilters : Initializes CAN1 filters        
* param  Remap configuration
* param  
* retval
*/
void Can1_InitializateFilters(void){	
	//todo: add some more features 	
	CAN1->FMR |= CAN_FMR_FINIT; // This bit starts Filter Initialization
	CAN1->FM1R &= ~0xFFFFFFFF;
	CAN1->FS1R &= ~0xFFFFFFFF;
	CAN1->FFA1R &= ~0xFFFFFFFF;
	CAN1->FA1R	&= ~0xFFFFFFFF;
	CAN1->sFilterRegister[0].FR1 &= ~0xFFFFFFFF; // CAN->F0R1
	CAN1->sFilterRegister[0].FR2 &= ~0xFFFFFFFF;
			
	CAN1->FM1R  |=  0x00; // 0 - Mask mode
	CAN1->FS1R  |=  0x01; // 1 - 32-bit
	CAN1->FFA1R |=  0x00; // 0 - FIFO 0
	CAN1->FA1R	|=  0x01;	// 1 - Active		
	CAN1->sFilterRegister[0].FR1 |=  0x00; // As long as we accept all the traffic filter value is 0
	CAN1->sFilterRegister[0].FR2 |=  0x00;		
	//Filter Initialization completed		
	CAN1->FMR &= ~CAN_FMR_FINIT;	
}

/**
 * brief  Can1_SetTxMessage: set up an empty mailbox with a single message (with dlc, id and data) 
 * param  CanMessage, Mailbox number
 * param  
 * retval 0 - OK, 1 - Err
 */	
uint32_t Can1_SetTxMessage(Std_Can_Message_Type canMessage, uint32_t mailboxNo){
	CAN1->sTxMailBox[mailboxNo].TIR	  &= ~ 0xFFFFFFFF;  // Clean up
	CAN1->sTxMailBox[mailboxNo].TDTR  &= ~ 0xFFFFFFFF;	// Clean up
	CAN1->sTxMailBox[mailboxNo].TDLR  &= ~ 0xFFFFFFFF;  // Clean up
	CAN1->sTxMailBox[mailboxNo].TDHR  &= ~ 0xFFFFFFFF;  // Clean up
	
// ID value is set within 31-21 bit range. First set bits 28-21
	CAN1->sTxMailBox[mailboxNo].TIR  |= (canMessage.id_lowbyte << 21);
	canMessage.id_highbyte &=0x1F;	// Discard bits 7-3 of the second identificator part
//set bits 31-28		
	CAN1->sTxMailBox[mailboxNo].TIR  |= (canMessage.id_highbyte << 29);			
	CAN1->sTxMailBox[mailboxNo].TDTR |= canMessage.dlc; 
	
	uint8_t MessageSize;
	MessageSize=canMessage.dlc;	// Data fields assignation is based on the message size																				
	if(MessageSize>8){		
		return 1;  //make sure DLC is not greater, than 8
	}	
	if(MessageSize>=1){
		CAN1->sTxMailBox[mailboxNo].TDLR |= canMessage.data[0];}					  	 // DATA0
	if(MessageSize>=2){
		CAN1->sTxMailBox[mailboxNo].TDLR |= (canMessage.data[1] << 8);} 				// DATA1	
	if(MessageSize>=3){
		CAN1->sTxMailBox[mailboxNo].TDLR |= (canMessage.data[2] << 16);} 				// DATA2
	if(MessageSize>=4){	
		CAN1->sTxMailBox[mailboxNo].TDLR |= (canMessage.data[3] << 24);} 				// DATA3
	if(MessageSize>=5){
		CAN1->sTxMailBox[mailboxNo].TDHR |= canMessage.data[4];} 								// DATA4	
	if(MessageSize>=6){
		CAN1->sTxMailBox[mailboxNo].TDHR |= (canMessage.data[5] << 8);} 				// DATA5
	if(MessageSize>=7){
		CAN1->sTxMailBox[mailboxNo].TDHR |= (canMessage.data[6] << 16);} 				// DATA6		
	if(MessageSize==8){
		CAN1->sTxMailBox[mailboxNo].TDHR |= (canMessage.data[7] << 24);} 				// DATA7 	
	return 0;			
	}
	

/**
* brief  Can1_GetEmptyMailbox: find an empty mailbox
* param  CurrentMailBox
* param  
* retval Empty mailbox number otherwise - Error number
*/
/* This function is designed to be able to work with a continuous CAN-traffic flow */
/* With every new transmission in the first place this function checks for the next mailbox free*/
/* E.g. If mailbox0 was used for a previous transmission, this function looks for 	*/
/* an empty mailbox starting from mailbox1,then mailbox2, then mailbox0	*/
uint32_t Can1_GetEmptyMailbox(){		
	uint32_t EmptyMailBoxNo;
  if(CAN1->TSR == 0x1C000000){ //The very first message after reset
		EmptyMailBoxNo=0;
		return EmptyMailBoxNo;
	}			
	for (int i=0; i < CAN_EMPTY_MAILBOX_TIMEOUT; i++){
		if((CAN1->TSR & CAN_TSR_TME0) && (CAN1->TSR & CAN_TSR_RQCP2)) 		 {EmptyMailBoxNo= CAN_TX_MAILBOX_0;}
		else if((CAN1->TSR & CAN_TSR_TME1) && (CAN1->TSR & CAN_TSR_RQCP0)) {EmptyMailBoxNo= CAN_TX_MAILBOX_1;}
		else if((CAN1->TSR & CAN_TSR_TME2) && (CAN1->TSR & CAN_TSR_RQCP1)) {EmptyMailBoxNo= CAN_TX_MAILBOX_2;}			
		}
	if(!(CAN1->TSR & CAN_TSR_TME0) && !(CAN1->TSR & CAN_TSR_TME1) && !(CAN1->TSR & CAN_TSR_TME2)){
		EmptyMailBoxNo=ERR_CAN1_NO_EMPTY_MAILBOX;//no empty mailbox found
	}			
	return EmptyMailBoxNo;
}

	
	
/**
* brief  Can1_Transmit_StdMessage_8b : transmit a 8-byte standart id message via CAN1        
* param  CanMessage
* param  
* retval 0 OK 1 - Err
*/
	
uint32_t Can1_Transmit_StdMessage(Std_Can_Message_Type canMessage){
	uint32_t mailboxNo;
	mailboxNo =	Can1_GetEmptyMailbox();	
	if(mailboxNo==ERR_CAN1_NO_EMPTY_MAILBOX){
		return 1;
	}
	Can1_SetTxMessage(canMessage, mailboxNo);
	CAN1->sTxMailBox[mailboxNo].TIR  |= CAN_TI0R_TXRQ; // Command to send
	return 0;
}

	
/**
* brief  Can1_Recieve_StdMessage_8b : transmit a 8-byte standart id message via CAN1        
* param  FifoNo Fifo number 0 or 1
* param  
* retval 0 OK 1 - Err
*/	
	
Std_Can_Message_Type Can1_Recieve_StdMessage(uint32_t FifoNo){
	static Std_Can_Message_Type recievedMessage;  
			
	recievedMessage.id_lowbyte  =(CAN1->sFIFOMailBox[FifoNo].RIR >> 21);
	recievedMessage.id_highbyte =(CAN1->sFIFOMailBox[FifoNo].RIR >> 29);
	recievedMessage.dlc         = CAN1->sFIFOMailBox[FifoNo].RDTR; 
	uint8_t MessageSize;
	MessageSize=recievedMessage.dlc;	// Data fields assignation is based on the message size		

	if(MessageSize>=1){
		recievedMessage.data[0]			= CAN1->sFIFOMailBox[FifoNo].RDLR;}					 		 	 // DATA0
	if(MessageSize>=2){
		recievedMessage.data[1]			=(CAN1->sFIFOMailBox[FifoNo].RDLR >> 8);} 					// DATA1	
	if(MessageSize>=3){
		recievedMessage.data[2]			=(CAN1->sFIFOMailBox[FifoNo].RDLR >> 16);} 					// DATA2
	if(MessageSize>=4){	
		recievedMessage.data[3]			=(CAN1->sFIFOMailBox[FifoNo].RDLR >> 24);} 					// DATA3
	if(MessageSize>=5){
		recievedMessage.data[4]			= CAN1->sFIFOMailBox[FifoNo].RDHR;} 								// DATA4	
	if(MessageSize>=6){
		recievedMessage.data[5]			=(CAN1->sFIFOMailBox[FifoNo].RDHR >> 8);} 					// DATA5
	if(MessageSize>=7){
		recievedMessage.data[6]			=(CAN1->sFIFOMailBox[FifoNo].RDHR >> 16);} 					// DATA6		
	if(MessageSize==8){
		recievedMessage.data[7]			=(CAN1->sFIFOMailBox[FifoNo].RDHR >> 24);} 					// DATA7 	
		
	uint8_t regCounter; 															// Set all unused fields with 0xff		
	for(regCounter = 8; regCounter > MessageSize;){
		recievedMessage.data[--regCounter]=0xff;
	}				
	CAN1->RF0R |= CAN_RF0R_RFOM0; //release		
	return recievedMessage;
}
	
/**
* brief  Can1_Check_Fifo0Pending_StdMessage : check if FIFO 0 empty        
* param  
* param  
* retval 0 empty 1 - not empty
*/	
	

uint32_t Can1_Check_Fifo0Pending_StdMessage(void){		
	if(CAN1->RF0R & CAN_RF0R_FMP0){
		return 1;
	}
	else return 0;
}
	
	void FlushCanTxBuffers()	{
 for(int i=0; i<3; i++){
	CAN1->sTxMailBox[i].TIR	 = 0; 
	CAN1->sTxMailBox[i].TDTR =0; 		
	CAN1->sTxMailBox[i].TDLR = 0;  
	CAN1->sTxMailBox[i].TDHR = 0;}
}


