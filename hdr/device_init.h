#ifndef DEVICE_INIT_H_INCLUDED
#define DEVICE_INIT_H_INCLUDED

#include "stm32f10x.h"
#include "string_convertor.h"
#include "usart.h"

#define USB_CONFIG 0          															// USB is not used
#define INDICATOR_LED_ON     GPIOC->BSRR = GPIO_BSRR_BR13		// Toggle evaluation board LED on
#define INDICATOR_LED_OFF    GPIOC->BSRR = GPIO_BSRR_BS13		// Toggle evaluation board LED off

 
 
 
 
//********************* ERROR CODES *********************//
#define ERROR_NULL																				0
#define ERR_CAN_LOOP_BUFFER_OVERFLOW 											1
//********************* ERROR CODES *********************//

//********************* STATUS CODES ********************//
#define STAT_NULL																					0
//********************* STATUS CODES ********************//

	uint32_t ClockInit(void);														// Configuring initial clocks and frequences
	void EnablePeripherals(void);												// Choose the peripherals to be used in the project and set clocking
	uint32_t GetProgrammStatus(uint16_t StatusNumber);	// Diagnostics function (status)
	uint32_t HandleProgrammError(uint16_t ErrorNumber); // Diagnostics function (error)

#endif /* DEVICE_INIT_H_INCLUDED*/
