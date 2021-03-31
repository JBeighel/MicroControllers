/**	#File Information
		File:	UART_NucleoL412KB.c
		Author:	J. Beighel
		Date:	2021-03-31
*/


/***** Includes		*****/
	#include "UART_NucleoL412KB.h"

/***** Definitions	*****/

	
/***** Constants	*****/


/***** Globals		*****/
	sNucleoUART_t gSTUart1 = { .pHWInfo = &huart1, };
	sNucleoUART_t gSTUart2 = { .pHWInfo = &huart2, };

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

	#ifdef UART_STINTERRUPT
		sNucleoUART_t *pSTUart = (sNucleoUART_t *)pHWInfo;

		// Reset buffer indexes (clears the buffer) in pHWInfo as well
		pSTUart->nIdxStart = 0;
		pSTUart->nIdxStop = 0;

		//Start interrupt receiving
		UART_Start_Receive_IT(pSTUart->pHWInfo, &(pSTUart->aRXBuff[pSTUart->nIdxStop]), 1);
	#endif

	return UART_Success;
}

eUARTReturn_t NucleoUARTShutdown(sUARTIface_t *pUARTIface) {
	//Ho means to shut down the hardware on Nucleo
	return UART_Fail_Unsupported;
}

eUARTReturn_t NucleoUARTReadData(sUARTIface_t *pUARTIface, uint16_t nDataSize, void *pDataBuff, uint16_t *pnBytesRead) {
	#ifdef UART_STINTERRUPT //Interrupt in use, read from buffer
		uint8_t *pBuff = pDataBuff; //Casting to a byte buffer
		sNucleoUART_t *pSTUart = (sNucleoUART_t *)pUARTIface->pHWInfo;
		uint32_t nCtr, nIdx;

		nIdx = pSTUart->nIdxStart;//start at first data byte
		for (nCtr = 0; nCtr < nDataSize; nCtr++) {
			if (nIdx == pSTUart->nIdxStop) { //Ran out of data in the buffer
				break;
			}

			pBuff[nCtr] = pSTUart->aRXBuff[nIdx];

			nIdx += 1;
			if (nIdx >= UART_RXBUFFSIZE) {
				nIdx = 0;
			}
		}

		pSTUart->nIdxStart = nIdx; //Set the start to the end of what was read
		(*pnBytesRead) = nCtr; //Report how many bytes were read
		return UART_Success;
	#else //No interrupt, read direct from port
		HAL_StatusTypeDef eResult;
			sNucleoUART_t *pSTUart = (sNucleoUART_t *)pUARTIface->pHWInfo;

			eResult = HAL_UART_Receive(pSTUart->pHWInfo, (uint8_t *)pDataBuff, nDataSize, UART_TIMEOUT);

			if (eResult == HAL_OK) {
				*pnBytesRead = nDataSize;
				return UART_Success;
			} else if (eResult == HAL_TIMEOUT) {
				*pnBytesRead = 0; //Don't know how many were actually received
				return UART_Warn_Timeout;
			} else {
				return UART_Fail_Unknown;
			}
	#endif
}

eUARTReturn_t NucleoUARTWriteData(sUARTIface_t *pUARTIface, uint16_t nDataSize, const void *pDataBuff) {
	HAL_StatusTypeDef eResult;
	sNucleoUART_t *pSTUart = (sNucleoUART_t *)pUARTIface->pHWInfo;

	eResult = HAL_UART_Transmit(pSTUart->pHWInfo, (uint8_t *)pDataBuff, nDataSize, UART_TIMEOUT);

	if (eResult == HAL_OK) {
		return UART_Success;
	} else {
		return UART_Fail_Unknown;
	}
}

eUARTReturn_t NucleoUARTDataAvailable(sUARTIface_t *pUARTIface, uint16_t *pnBytesAvailable) {
	#ifdef UART_STINTERRUPT //Interrupt enabled, report data in buffer
		sNucleoUART_t *pSTUart = (sNucleoUART_t *)pUARTIface->pHWInfo;

		if (pSTUart->nIdxStop > pSTUart->nIdxStart) {
			(*pnBytesAvailable) = pSTUart->nIdxStop - pSTUart->nIdxStart;
		} else {
			(*pnBytesAvailable) = UART_RXBUFFSIZE - pSTUart->nIdxStop; //Bytes at end of buffer
			(*pnBytesAvailable) += pSTUart->nIdxStart; //Bytes at start of buffer
		}

		return UART_Success;
	#else
		return UART_Fail_Unsupported; //Only supported with interrupt filled buffer
	#endif
}

eUARTReturn_t NucleoUARTWaitDataSend(sUARTIface_t *pUARTIface) {
	//((HardwareSerial *)pUARTIface->pHWInfo)->flush();
	return UART_Fail_Unsupported;
}

#ifdef UART_STINTERRUPT
	void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
		sNucleoUART_t *pSTUart;

		//First determine which UART this event is for
		if (huart == gSTUart1.pHWInfo) { //Data received on UART 1
			pSTUart = &gSTUart1;
		} else if (huart == gSTUart2.pHWInfo) { //Data received on UART 2
			pSTUart = &gSTUart2;
		} else { //Unknown UART?
			return;
		}

		//One character was received and was placed in aRXBuff, update the ring buffer
		pSTUart->nIdxStop += 1; //Advance the end of the ring buffer
		if (pSTUart->nIdxStop >= UART_RXBUFFSIZE) {
			pSTUart->nIdxStop = 0; //Reached the end, wrap to beginning
		}

		if (pSTUart->nIdxStop == pSTUart->nIdxStart) { //Reached beginning of data, start over writing
			pSTUart->nIdxStart += 1;
			if (pSTUart->nIdxStart >= UART_RXBUFFSIZE) {
				pSTUart->nIdxStart = 0; //Reached the end, wrap to beginning
			}
		}

		//Lets collect the next byte
		UART_Start_Receive_IT(pSTUart->pHWInfo, &(pSTUart->aRXBuff[pSTUart->nIdxStop]), 1);

		return;
	}
#endif
