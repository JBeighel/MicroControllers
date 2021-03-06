/**	@defgroup	uartifcae_arduino
	@brief		UART General Interface implementation for Arduino
	@details	v0.3
	# Description #
	
	# Usage #
	
	# File Information #
		File:	UART_Arduino.h
		Author:	J. Beighel
		Date:	2021-01-22
*/

#ifndef __NAME_H
	#define __NAME_H

/***** Includes		*****/
	#include <HardwareSerial.h>
	
	#include "UARTGeneralInterface.h"

/***** Definitions	*****/
	#define UART_1_HWINFO (&Serial)
	
	#define UART_2_HWINFO (&Serial1)
	
	#define		BUILD_DEBUG		1
	#ifdef BUILD_DEBUG
		#define	DEBUG_PRINT	Serial.print
	#endif
	
	#ifdef UART_INIT
		#undef UART_INIT
	#endif
	#define UART_INIT	ArduinoUARTPortInitialize
	
	#ifndef mSecDelay
		#define mSecDelay	delay
	#endif
	
	#define UART_1_CAPS		(UART_Configure | UART_Shutdown | UART_ReadData | UART_WriteData | UART_DataAvailable | UART_DataWaitSend)
	
	#define UART_2_CAPS		(UART_Configure | UART_Shutdown | UART_ReadData | UART_WriteData | UART_DataAvailable | UART_DataWaitSend)
	
/***** Constants	*****/


/***** Globals		*****/


/***** Prototypes 	*****/
	eUARTReturn_t ArduinoUARTPortInitialize(sUARTIface_t *pUARTIface, uint32_t nBaudRate, eUARTModes_t eReqMode, void *pHWInfo);
	
	eUARTReturn_t ArduinoUARTShutdown(sUARTIface_t *pUARTIface);
		
	eUARTReturn_t ArduinoUARTReadData(sUARTIface_t *pUARTIface, uint16_t nDataSize, void *pDataBuff, uint16_t *pnBytesRead);
	eUARTReturn_t ArduinoUARTWriteData(sUARTIface_t *pUARTIface, uint16_t nDataSize, const void *pDataBuff);
	eUARTReturn_t ArduinoUARTDataAvailable(sUARTIface_t *pUARTIface, uint16_t *pnBytesAvailable);
	eUARTReturn_t ArduinoUARTWaitDataSend(sUARTIface_t *pUARTIface);

/***** Functions	*****/

eUARTReturn_t ArduinoUARTPortInitialize(sUARTIface_t *pUARTIface, uint32_t nBaudRate, eUARTModes_t eReqMode, void *pHWInfo) {
	byte Mode;
	
	UARTInterfaceInitialize(pUARTIface); //Make sure all function pointers are valid
	
	//Verify the mode requested
	switch (eReqMode) {
		#if !defined(ARDUINO_NUCLEO_L412KB)
		case UART_5None1 :
			Mode = SERIAL_5N1;
			break;
		#endif
		
		#if !defined(ARDUINO_NUCLEO_L412KB)
		case UART_6None1 : 
			Mode = SERIAL_6N1;
			break;
		#endif
		
		case UART_7None1 : 
			Mode = SERIAL_7N1;
			break;
		case UART_8None1 : 
			Mode = SERIAL_8N1;
			break;
		
		#if !defined(ARDUINO_NUCLEO_L412KB)
		case UART_5None2 : 
			Mode = SERIAL_5N2;
			break;
		#endif
			
		#if !defined(ARDUINO_NUCLEO_L412KB)
		case UART_6None2 : 
			Mode = SERIAL_6N2;
			break;
		#endif
			
		case UART_7None2 : 
			Mode = SERIAL_7N2;
			break;
		case UART_8None2 : 
			Mode = SERIAL_8N2;
			break;
			
		#if !defined(ARDUINO_NUCLEO_L412KB)
		case UART_5Even1 : 
			Mode = SERIAL_5E1;
			break;
		#endif	
		
		#if !defined(ARDUINO_NUCLEO_L412KB)
		case UART_6Even1 : 
			Mode = SERIAL_6E1;
			break;
		#endif
		
		case UART_7Even1 : 
			Mode = SERIAL_7E1;
			break;
		case UART_8Even1 : 
			Mode = SERIAL_8E1;
			break;
		
		#if !defined(ARDUINO_NUCLEO_L412KB)
		case UART_5Even2 : 
			Mode = SERIAL_5E2;
			break;
		#endif
		
		#if !defined(ARDUINO_NUCLEO_L412KB)
		case UART_6Even2 : 
			Mode = SERIAL_6E2;
			break;
		#endif
		
		case UART_7Even2 : 
			Mode = SERIAL_7E2;
			break;
		case UART_8Even2 : 
			Mode = SERIAL_8E2;
			break;
		
		#if !defined(ARDUINO_NUCLEO_L412KB)
		case UART_5Odd1 : 
			Mode = SERIAL_5O1;
			break;
		#endif

		#if !defined(ARDUINO_NUCLEO_L412KB)
		case UART_6Odd1 : 
			Mode = SERIAL_6O1;
			break;
		#endif
		
		case UART_7Odd1 : 
			Mode = SERIAL_7O1;
			break;
		case UART_8Odd1 : 
			Mode = SERIAL_8O1;
			break;
			
		#if !defined(ARDUINO_NUCLEO_L412KB)
		case UART_5Odd2 : 
			Mode = SERIAL_5O2;
			break;
		#endif
		
		case UART_6Odd2 : 
			Mode = SERIAL_6O2;
			break;
		case UART_7Odd2 : 
			Mode = SERIAL_7O2;
			break;
		case UART_8Odd2 : 
			Mode = SERIAL_8O2;
			break;
		default :
			return UART_Fail_Unsupported;
	}
	
	//Setup the hardware
	((HardwareSerial *)pHWInfo)->begin(nBaudRate, Mode);
	
	//Setup the interface values
	pUARTIface->pfPortInitialize = &ArduinoUARTPortInitialize;
	pUARTIface->pfShutdown = &ArduinoUARTShutdown;
	pUARTIface->pfUARTReadData = &ArduinoUARTReadData;
	pUARTIface->pfUARTWriteData = &ArduinoUARTWriteData;
	pUARTIface->pfUARTDataAvailable = &ArduinoUARTDataAvailable;
	pUARTIface->pfUARTWaitDataSend = &ArduinoUARTWaitDataSend;
	
	pUARTIface->nBaudRate = nBaudRate;
	pUARTIface->eMode = eReqMode;
	pUARTIface->pHWInfo = pHWInfo;
	
	return UART_Success;
}
	
eUARTReturn_t ArduinoUARTShutdown(sUARTIface_t *pUARTIface) {
	//Ho means to shut down the hardware on Arduino
	return UART_Fail_Unsupported;
}
	
eUARTReturn_t ArduinoUARTReadData(sUARTIface_t *pUARTIface, uint16_t nDataSize, void *pDataBuff, uint16_t *pnBytesRead) {
	(*pnBytesRead) = ((HardwareSerial *)pUARTIface->pHWInfo)->readBytes((char *)pDataBuff, nDataSize);
	
	return UART_Success;
}

eUARTReturn_t ArduinoUARTWriteData(sUARTIface_t *pUARTIface, uint16_t nDataSize, const void *pDataBuff) {
	((HardwareSerial *)pUARTIface->pHWInfo)->write((char *)pDataBuff, nDataSize);
	
	return UART_Success;
}

eUARTReturn_t ArduinoUARTDataAvailable(sUARTIface_t *pUARTIface, uint16_t *pnBytesAvailable) {
	(*pnBytesAvailable) = ((HardwareSerial *)pUARTIface->pHWInfo)->available();
	
	return UART_Success;
}

eUARTReturn_t ArduinoUARTWaitDataSend(sUARTIface_t *pUARTIface) {
	((HardwareSerial *)pUARTIface->pHWInfo)->flush();
	return UART_Success;
}

#endif

