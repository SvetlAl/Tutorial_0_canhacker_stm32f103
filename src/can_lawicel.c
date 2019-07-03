//*****************************************************************
// Some string functions to work with CanHacker
// Since all the commands are processed in an ascii format, some conversion is neccessary
// July 2019 std id messages Recieve/transmit command supported
// todo: add timestamp, extended id support
// Alex Svetlichnyy 2019
//*****************************************************************

#include "can_lawicel.h"

extern uint8_t LawicelCommandCounter;													// Byte counter for lawicel command buffer
extern uint8_t LawicelBuffer[LAWICEL_COMMAND_BUFFER_SIZE];		// Lawicel command recieved from the host marked  

/**
* brief  Send_StdLawicelMessage: Send a CAN message to the host in lawicel format
* brief  tIIILD'\r'  where 't' - message command mark III - id numbers L - length D-DDDDDDDD data bytes(0-8)
* param  Std_Can_Message_Type
* param  
* retval 
*/
/* Message can take up from 5 to 21 (+'\r') bytes depending on data size  */
void Send_Std_LawicelMessage(Std_Can_Message_Type CanMessage){
	static uint8_t lawicelMessage_toSend[22];
	uint8_t commandSize;
		lawicelMessage_toSend[0]='t';																								// Lawicel 'message' mark
		lawicelMessage_toSend[1]=halfbyte_to_hexascii(CanMessage.id_highbyte);			// BXX byte of a std id
		lawicelMessage_toSend[2]=halfbyte_to_hexascii(CanMessage.id_lowbyte >> 4);	// XBX byte of a std id
		lawicelMessage_toSend[3]=halfbyte_to_hexascii(CanMessage.id_lowbyte);				// XXB byte of a std id
		lawicelMessage_toSend[4]=halfbyte_to_hexascii(CanMessage.dlc);							// DLC data length
		commandSize=CanMessage.dlc;																									// Number of message data bytes	
		commandSize=commandSize*2;																									// Each byte is split into 2 'ascii' format bytes
		commandSize=commandSize+4;																									// Skip first 0-4 bytes before count
		uint8_t lastByte=commandSize+1;																							// Mark the last byte position
		uint32_t messageSize=lastByte+1;																						// Message size calculation (lastbyte position + zerobyte)
	
		uint8_t messageByteCounter=0;
		if(commandSize==20){
			lawicelMessage_toSend[commandSize--]=halfbyte_to_hexascii(CanMessage.data[messageByteCounter]);	         
			lawicelMessage_toSend[commandSize--]=halfbyte_to_hexascii(CanMessage.data[messageByteCounter++] >> 4);}   
		if(commandSize==18){
			lawicelMessage_toSend[commandSize--]=halfbyte_to_hexascii(CanMessage.data[messageByteCounter]);										
			lawicelMessage_toSend[commandSize--]=halfbyte_to_hexascii(CanMessage.data[messageByteCounter++] >> 4);}	      			
		if(commandSize==16){
			lawicelMessage_toSend[commandSize--]=halfbyte_to_hexascii(CanMessage.data[messageByteCounter]);										
			lawicelMessage_toSend[commandSize--]=halfbyte_to_hexascii(CanMessage.data[messageByteCounter++] >> 4);}							
		if(commandSize==14){
			lawicelMessage_toSend[commandSize--]=halfbyte_to_hexascii(CanMessage.data[messageByteCounter]);										
			lawicelMessage_toSend[commandSize--]=halfbyte_to_hexascii(CanMessage.data[messageByteCounter++] >> 4);}							
		if(commandSize==12){
			lawicelMessage_toSend[commandSize--]=halfbyte_to_hexascii(CanMessage.data[messageByteCounter]);											
			lawicelMessage_toSend[commandSize--]=halfbyte_to_hexascii(CanMessage.data[messageByteCounter++] >> 4);}										
		if(commandSize==10){
			lawicelMessage_toSend[commandSize--]=halfbyte_to_hexascii(CanMessage.data[messageByteCounter]);											
			lawicelMessage_toSend[commandSize--]=halfbyte_to_hexascii(CanMessage.data[messageByteCounter++] >> 4);}									
		if(commandSize==8){
			lawicelMessage_toSend[commandSize--]=halfbyte_to_hexascii(CanMessage.data[messageByteCounter]);											
			lawicelMessage_toSend[commandSize--]=halfbyte_to_hexascii(CanMessage.data[messageByteCounter++] >> 4);}								
		if(commandSize==6){
			lawicelMessage_toSend[commandSize--]=halfbyte_to_hexascii(CanMessage.data[messageByteCounter]);											
			lawicelMessage_toSend[commandSize--]=halfbyte_to_hexascii(CanMessage.data[messageByteCounter++] >> 4);}
			
			lawicelMessage_toSend[lastByte]='\r';		
			Dma1Usart1_SendByteArray(lawicelMessage_toSend, messageSize);
}
/**
* brief  Lawicel_Ititialize: CanHacker initializes connection with a version request 'V','v'
* brief  answer format is VXXXX, vXXXXX. All the other requests are responded with '\r'
* param  recieved data
* param  
* retval 
*/
/* Message can take up from 5 to 21 (+'\r') bytes depending on data size  */
void Lawicel_SendInstruction(uint8_t *data){ 
	uint8_t Buf[7];
	if(data[0]=='V'){
	 Buf[0]='V';
	 Buf[1]='1';
	 Buf[2]='0';
	 Buf[3]='1';
	 Buf[4]='0';
	 Buf[5]='\r';	
   Dma1Usart1_SendByteArray(Buf,6);
		}
	else if(data[0]=='v'){
	 Buf[0]='v';
	 Buf[1]='S';
	 Buf[2]='T';
	 Buf[3]='M';
	 Buf[4]='3';
	 Buf[5]='2';	
	 Buf[6]='\r';	
   Dma1Usart1_SendByteArray(Buf,7);			
		} 
	else {
	 Buf[0]='\r';
	 Dma1Usart1_SendByte(&Buf[0]);}	
}
/**
* brief  Lawicel_ProcessCommand: this function decides what to do with the data recieved from the host
* brief  
* param  data recieved from the host
* param  
* retval 
*/
/*   */
void Lawicel_ProcessCommand(uint8_t *data){
	if(data[0]=='V'){
		Lawicel_SendInstruction(data);																//'V' version request
	}
	else if(data[0]=='v'){
		Lawicel_SendInstruction(data);	
	} 
	else if(data[0]=='t'){																		//'t' means, that upcoming bytes contain CAN-message data 
		LawicelCommandCounter=1;																//Since the message length can vary, this counter is used to count message bytes
		LawicelBuffer[0]='t';		
	}
	else if(LawicelCommandCounter!=0){												// If a message counter is running, continue recieving data into the buffer
		if(data[0]=='\r'){																			// If a message contains less than 8 bytes data (recieved data is less than
			LawicelCommandCounter=0;															// max buffer size. Process message and reset the byte counter.
			Lawicel_ProcessTxCommand();}
		
		LawicelBuffer[LawicelCommandCounter]=data[0];						// If '\r' is not met, write put the byte into the buffer
		LawicelCommandCounter++;																// Increase the counter

		if(LawicelCommandCounter>LAWICEL_COMMAND_BUFFER_SIZE){	// Prevent buffer overflow 
			LawicelCommandCounter=0;															// If a message contains at least 8 bytes data (recieved data is at least
			Lawicel_ProcessTxCommand();														// equal to max buffer size. Process message and reset the byte counter.
		}
	}			
	else {Lawicel_SendInstruction(data);	}										// Something else was recieved from the host. Process it as an instruction
}

/**
* brief  Lawicel_ProcessTxCommand: construct a CAN message from ascii-string and send it to a CAN-Network
* brief  
* param  
* param  
* retval 0 OK 1 Err
*/
/* tIIILD'\r'  where 't' - message command mark III - id numbers L - length D-DDDDDDDD data bytes(0-8) */
uint32_t Lawicel_ProcessTxCommand(){
		Std_Can_Message_Type hostMessageToSend;		 					// Construct a CAN message to send using the data from the host
		uint8_t tmp;
																												// Zero byte is already set as 't'
		tmp=hexascii_to_halfbyte(LawicelBuffer[1]);					// Set a Byte1 as BXX byte of a std id
		hostMessageToSend.id_highbyte=tmp;                  // Set a Byte1 as BXX byte of a std id            
		
		tmp=hexascii_to_halfbyte(LawicelBuffer[2]);					// Set a Byte2 as XBX byte of a std id
		tmp=(tmp << 4);
		tmp=tmp | hexascii_to_halfbyte(LawicelBuffer[3]);		// Set a Byte3 as XXB byte of a std id
		hostMessageToSend.id_lowbyte=tmp;
		
		tmp=hexascii_to_halfbyte(LawicelBuffer[4]);					// Set a Byte4 as a dlc
		hostMessageToSend.dlc=tmp;                        
		uint8_t messageLength = (hostMessageToSend.dlc*2);	// Calculate a message length: every data byte was split into 2 ascii
		messageLength = messageLength+4;										// Skip 0-4 bytes of an ascii string
		uint8_t dataByteCounter=0;
	
		if(messageLength==20){			
			tmp = hexascii_to_halfbyte(LawicelBuffer[messageLength--]);								//20					
			tmp |= (hexascii_to_halfbyte(LawicelBuffer[messageLength--]) << 4 );			//19
			hostMessageToSend.data[dataByteCounter++]=tmp;														//0
		}			
		if(messageLength==18){			
			tmp = hexascii_to_halfbyte(LawicelBuffer[messageLength--]);								
			tmp |= (hexascii_to_halfbyte(LawicelBuffer[messageLength--]) << 4 );
			hostMessageToSend.data[dataByteCounter++]=tmp;			
		}			
		if(messageLength==16){			
			tmp = hexascii_to_halfbyte(LawicelBuffer[messageLength--]);							
			tmp |= (hexascii_to_halfbyte(LawicelBuffer[messageLength--]) << 4 );
			hostMessageToSend.data[dataByteCounter++]=tmp;			
		}		
		if(messageLength==14){			
			tmp = hexascii_to_halfbyte(LawicelBuffer[messageLength--]);							
			tmp |= (hexascii_to_halfbyte(LawicelBuffer[messageLength--]) << 4 );
			hostMessageToSend.data[dataByteCounter++]=tmp;			
		}	
		if(messageLength==12){			
			tmp = hexascii_to_halfbyte(LawicelBuffer[messageLength--]);							
			tmp |= (hexascii_to_halfbyte(LawicelBuffer[messageLength--]) << 4 );
			hostMessageToSend.data[dataByteCounter++]=tmp;			
		}		
		if(messageLength==10){			
			tmp = hexascii_to_halfbyte(LawicelBuffer[messageLength--]);							
			tmp |= (hexascii_to_halfbyte(LawicelBuffer[messageLength--]) << 4 );
			hostMessageToSend.data[dataByteCounter++]=tmp;			
		}		
		if(messageLength==8){			
			tmp = hexascii_to_halfbyte(LawicelBuffer[messageLength--]);									//8
			tmp |= (hexascii_to_halfbyte(LawicelBuffer[messageLength--]) << 4 );				//7
			hostMessageToSend.data[dataByteCounter++]=tmp;															//0
		}		
		if(messageLength==6){			
			tmp = hexascii_to_halfbyte(LawicelBuffer[messageLength--]);									//6
			tmp |= (hexascii_to_halfbyte(LawicelBuffer[messageLength--]) << 4 );				//5
			hostMessageToSend.data[dataByteCounter++]=tmp;			
		}
		uint8_t MessageSize=hostMessageToSend.dlc;		 																						
		for(dataByteCounter = 8; dataByteCounter > MessageSize;){					// Set all unused fields with 0xff	
			hostMessageToSend.data[--dataByteCounter]=0xff;
		}		
		
		uint32_t transmissionResult;
		transmissionResult= Can1_Transmit_StdMessage(hostMessageToSend);
		return transmissionResult;		
}
	
	
