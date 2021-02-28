/***** Includes		*****/
	#include "UART_NucleoL412KB.h"

/***** Definitions	*****/

	
/***** Constants	*****/


/***** Globals		*****/


/***** Prototypes 	*****/


/***** Functions	*****/

eUARTReturn_t NucleoUARTPortInitialize(sUARTIface_t *pUARTIface, uint32_t nBaudRate, eUARTModes_t eReqMode, void *pHWInfo) {
	UARTInterfaceInitialize(pUARTIface); //Make sure all function pointers are valid

	//Hardware settings are made in the Cube, no real way to change them here

	//Setup the interface values
	pUARTIface->pfPortInitialize = &NucleoUARTPortInitialize;
	pUARTIface->pfShutdown = &NucleoUARTShutdown;
	pUARTIface->pfUARTReadData = &NucleoUARTReadData;
	pUARTIface->pfUARTWriteData = &NucleoUARTWriteData;
	pUARTIface->pfUARTDataAvailable = &NucleoUARTDataAvailable;
	pUARTIface->pfUARTWaitDataSend = &NucleoUARTWaitDataSend;

	pUARTIface->nBaudRate = nBaudRate;
	pUARTIface->eMode = eReqMode;
	pUARTIface->pHWInfo = pHWInfo;

	return UART_Success;
}

eUARTReturn_t NucleoUARTShutdown(sUARTIface_t *pUARTIface) {
	//Ho means to shut down the hardware on Nucleo
	return UART_Fail_Unsupported;
}

eUARTReturn_t NucleoUARTReadData(sUARTIface_t *pUARTIface, uint16_t nDataSize, void *pDataBuff, uint16_t *pnBytesRead) {
	HAL_StatusTypeDef eResult;

	eResult = HAL_UART_Receive((UART_HandleTypeDef *)pUARTIface->pHWInfo, (uint8_t *)pDataBuff, nDataSize, UART_TIMEOUT);

	if (eResult == HAL_OK) {
		*pnBytesRead = nDataSize;
		return UART_Success;
	} else if (eResult == HAL_TIMEOUT) {
		*pnBytesRead = 0; //Dunno how many were actually received
		return UART_Warn_Timeout;
	} else {
		return UART_Fail_Unknown;
	}
}

eUARTReturn_t NucleoUARTWriteData(sUARTIface_t *pUARTIface, uint16_t nDataSize, const void *pDataBuff) {
	HAL_StatusTypeDef eResult;

	eResult = HAL_UART_Transmit((UART_HandleTypeDef *)pUARTIface->pHWInfo, (uint8_t *)pDataBuff, nDataSize, UART_TIMEOUT);

	if (eResult == HAL_OK) {
		return UART_Success;
	} else {
		return UART_Fail_Unknown;
	}
}

eUARTReturn_t NucleoUARTDataAvailable(sUARTIface_t *pUARTIface, uint16_t *pnBytesAvailable) {
	//(*pnBytesAvailable) = ((HardwareSerial *)pUARTIface->pHWInfo)->available();

	return UART_Fail_Unsupported;
}

eUARTReturn_t NucleoUARTWaitDataSend(sUARTIface_t *pUARTIface) {
	//((HardwareSerial *)pUARTIface->pHWInfo)->flush();
	return UART_Fail_Unsupported;
}
