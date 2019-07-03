//*****************************************************************
// Some functions to work with the strings, arrays and calculations
// 
// 
// 
//*****************************************************************

#ifndef STRING_CONVERTOR_H_INCLUDED
#define STRING_CONVERTOR_H_INCLUDED

#include "stm32f10x.h"


//***************** RGB LED DEFINITIONS *******************
#define RANGE_IN_MIN 100
#define RANGE_IN_MAX 1200
#define RANGE_RGB_MIN 0
#define RANGE_RGB_MAX 255
//***************** RGB LED DEFINITIONS *******************

uint16_t MergeTwoBytes_into_Uint16(unsigned char *Highbyte, unsigned char *Lowbyte);  // input char 'A'+'B' = output uint16_t 'AB'
void Convert_Char_into_String3(unsigned char char255, unsigned char *String3); 				// input char 'ABC' = output unsigned char[3] {C,B,A} 
void Convert_Uint16_into_String5(uint16_t input16t, unsigned char *String5); 					// input uint16_t 'ABCDE' = output unsigned char[5] {E,D,C,B,A}
	
unsigned char Translate_Uint16t_wRatio_Uint8t(uint16_t integer16, uint16_t rangeMin, uint16_t rangeMax); // Translate an uint16_t into uint8_t in proportion to a range
unsigned char GetAverageValueFrom_3uint8(unsigned char i1, unsigned char i2,unsigned char i3); // Average value out from 3-bytes sum 3

uint8_t halfbyte_to_hexascii(uint8_t _halfbyte); // Translate a single hex (0-F) into an ascii symbol 
uint8_t hexascii_to_halfbyte(uint8_t _ascii);		 // Translate an ascii symbol into a hex (0-F)

#endif /* STRING_CONVERTOR_H_INCLUDED*/
