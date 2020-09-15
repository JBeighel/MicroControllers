/*
	File:	UARTGeneralInterface.c
	Author:	J. Beighel
	Created:09-15-2020
*/

/*****	Includes	*****/
#include "UARTGeneralInterface.h"


/*****	Definitions	*****/


/*****	Constants	*****/


/*****	Globals		*****/


/*****	Prototypes 	*****/


/*****	Functions	*****/

eUARTReturns_t UARTInterfaceInitialize(sUARTIface_t *pUARTIface) {
	pUARTIface->pfShutdown = &UARTShutdown;
	pUARTIface->pfPortInitialize = &UARTPortInitialize;
	pUARTIface->pfUARTReadData = &UARTReadData;
	pUARTIface->pfUARTWriteData = &UARTWriteData;
	pUARTIface->pfUARTDataAvailable = &UARTDataAvailable;
	pUARTIface->pfUARTWaitDataSend = &UARTWaitDataSend;
	
	return UART_Success;
}

eUARTReturns_t UARTPortInitialize(sUARTIface_t *pUARTIface, uint32_t nBaudRate, eUARTModes_t eMode, void *pHWInfo) {
	return UART_Fail_Unsupported;
}

eUARTReturns_t UARTShutdown(sUARTIface_t *pUARTIface) {
	return UART_Fail_Unsupported;
}
	
eUARTReturns_t UARTReadData(sUARTIface_t *pUARTIface, uint16_t nBuffSize, void *pDataBuff, uint16_t *pnBytesRead) {
	return UART_Fail_Unsupported;
}

eUARTReturns_t UARTWriteData(sUARTIface_t *pUARTIface, uint16_t nBuffSize, const void *pDataBuff) {
	return UART_Fail_Unsupported;
}

eUARTReturns_t UARTDataAvailable(sUARTIface_t *pUARTIface, uint16_t *pnBytesAvailable) {
	return UART_Fail_Unsupported;
}

eUARTReturns_t UARTWaitDataSend(sUARTIface_t *pUARTIface) {
	return UART_Fail_Unsupported;
}