/**	File:	Terminal.c
	Author:	J. Beighel
	Date:	2021-03-17
*/

/*****	Includes	*****/
	#include "Terminal.h"

/*****	Defines		*****/


/*****	Definitions	*****/


/*****	Constants	*****/


/*****	Globals		*****/


/*****	Prototypes 	*****/
eReturn_t IOCnctReadByteNoImplement(sIOConnect_t *pIOObj, uint8_t *pnByte);
eReturn_t IOCnctWriteByteNoImplement(sIOConnect_t *pIOObj, uint8_t nByte);
eReturn_t IOCnctWriteTextNoImplement(sIOConnect_t *pIOObj, char *pText);
eReturn_t IOCnctReadDataNoImplement(sIOConnect_t *pIOObj, uint8_t *pnDataBuff, uint32_t nBuffSize, uint32_t *pnReadSize);
eReturn_t IOCnctWriteDataNoImplement(sIOConnect_t *pIOObj, uint8_t *pnData, uint32_t nDataLen);

eReturn_t IOCnctReadByteUART(sIOConnect_t *pIOObj, uint8_t *pnByte);
eReturn_t IOCnctWriteByteUART(sIOConnect_t *pIOObj, uint8_t nByte);
eReturn_t IOCnctReadDataUART(sIOConnect_t *pIOObj, uint8_t *pnDataBuff, uint32_t nBuffSize, uint32_t *pnReadSize);
eReturn_t IOCnctWriteDataUART(sIOConnect_t *pIOObj, uint8_t *pnData, uint32_t nDataLen);

eReturn_t TerminalWriteTextLine(sTerminal_t *pTerminal, char *pText);
eReturn_t TerminalReadInput(sTerminal_t *pTerminal);

/*****	Functions	*****/
eReturn_t IOCnctObjectInitialize(sIOConnect_t *pIOObj) {
	pIOObj->pfReadByte = &IOCnctReadByteNoImplement;
	pIOObj->pfWriteByte = &IOCnctWriteByteNoImplement;
	pIOObj->pfReadData = &IOCnctReadDataNoImplement;
	pIOObj->pfWriteData = &IOCnctWriteDataNoImplement;
	
	pIOObj->pHWInfo = NULL;
	
	return Success;
}

eReturn_t IOCnctReadByteNoImplement(sIOConnect_t *pIOObj, uint8_t *pnByte) {
	return Fail_NotImplem;
}

eReturn_t IOCnctWriteByteNoImplement(sIOConnect_t *pIOObj, uint8_t nByte)  {
	return Fail_NotImplem;
}

eReturn_t IOCnctReadDataNoImplement(sIOConnect_t *pIOObj, uint8_t *pnDataBuff, uint32_t nBuffSize, uint32_t *pnReadSize) {
	eReturn_t eResult;
	uint32_t nCtr;

	*pnReadSize = 0; //Set initial value

	for (nCtr = 0; nCtr < nBuffSize; nCtr++) {
		eResult = pIOObj->pfReadByte(pIOObj, &(pnDataBuff[nCtr]));

		if (eResult != Success) {
			return eResult;
		}

		*pnReadSize += 1;
	}

	return Success;
}

eReturn_t IOCnctWriteDataNoImplement(sIOConnect_t *pIOObj, uint8_t *pnData, uint32_t nDataLen) {
	eReturn_t eResult;
	uint32_t nCtr;

	for (nCtr = 0; nCtr < nDataLen; nCtr++) {
		eResult = pIOObj->pfWriteByte(pIOObj, pnData[nCtr]);

		if (eResult != Success) {
			return eResult;
		}
	}

	return Success;
}

eReturn_t IOCnctCreateFromUART(sUARTIface_t *pUARTIface, sIOConnect_t *pIOObj) {
	IOCnctObjectInitialize(pIOObj);

	pIOObj->pfReadByte = &IOCnctReadByteUART;
	pIOObj->pfWriteByte = &IOCnctWriteByteUART;
	pIOObj->pfReadData = &IOCnctReadDataNoImplement;
	pIOObj->pfWriteData = &IOCnctWriteDataNoImplement;

	pIOObj->pHWInfo = pUARTIface;

	return Success;
}

eReturn_t IOCnctReadByteUART(sIOConnect_t *pIOObj, uint8_t *pnByte) {
	sUARTIface_t *pUART = (sUARTIface_t *)(pIOObj->pHWInfo);
	uint16_t nReadBytes;
	eUARTReturn_t eResult;

	eResult = pUART->pfUARTReadData(pUART, 1, pnByte, &nReadBytes);

	if (eResult < UART_Success) {
		return Fail_CommError;
	} else if ((eResult == UART_Warn_Timeout) || (nReadBytes < 1)) {
		return Warn_EndOfData;
	}

	return Success;
}

eReturn_t IOCnctWriteByteUART(sIOConnect_t *pIOObj, uint8_t nByte) {
	sUARTIface_t *pUART = (sUARTIface_t *)(pIOObj->pHWInfo);
	eUARTReturn_t eResult;

	eResult = pUART->pfUARTWriteData(pUART, 1, &nByte);

	if (eResult != UART_Success) {
		return Fail_CommError;
	}

	return Success;
}

eReturn_t IOCnctReadDataUART(sIOConnect_t *pIOObj, uint8_t *pnDataBuff, uint32_t nBuffSize, uint32_t *pnReadSize) {
	sUARTIface_t *pUART = (sUARTIface_t *)(pIOObj->pHWInfo);
	uint16_t nReadBytes, nDataLen;
	eUARTReturn_t eResult;

	while (nBuffSize > 0) {
		if (nBuffSize > UINT16_MAXVALUE) {
			nDataLen = UINT16_MAXVALUE;
		} else {
			nDataLen = nBuffSize;
		}

		eResult = pUART->pfUARTReadData(pUART, nDataLen, pnDataBuff, &nReadBytes);

		if (eResult < UART_Success) {
			return Fail_CommError;
		} else if ((eResult == UART_Warn_Timeout) || (nReadBytes < nDataLen)) {
			return Warn_EndOfData;
		}

		//REad successful, try to get more
		nBuffSize -= nDataLen;
		pnDataBuff = &(pnDataBuff[nDataLen]);
	}

	return Success;
}

eReturn_t IOCnctWriteDataUART(sIOConnect_t *pIOObj, uint8_t *pnData, uint32_t nDataLen) {
	sUARTIface_t *pUART = (sUARTIface_t *)(pIOObj->pHWInfo);
	eUARTReturn_t eResult;
	uint16_t nCurrWriteLen;

	while (nDataLen > 0) {
		if (nDataLen > UINT16_MAXVALUE) {
			nCurrWriteLen = UINT16_MAXVALUE;
		} else {
			nCurrWriteLen = nDataLen;
		}
		eResult = pUART->pfUARTWriteData(pUART, nCurrWriteLen, &pnData);

		if (eResult != UART_Success) {
			return Fail_CommError;
		}

		//Write successful, try to send more
		nDataLen -= nCurrWriteLen;
		pnData = &(pnData[nCurrWriteLen]);
	}

	return Success;
}

eReturn_t TerminalInitialize(sTerminal_t *pTerm, sIOConnect_t *pIOObj) {
	pTerm->pfWriteTextLine = &TerminalWriteTextLine;
	pTerm->pfReadInput = &TerminalReadInput;

	pTerm->pIOObj = pIOObj;
	pTerm->nBufferUsed = 0;

	return Success;
}

eReturn_t TerminalWriteTextLine(sTerminal_t *pTerminal, char *pText) {
	uint32_t nTextLen;
	eReturn_t eResult;

	nTextLen = strlen(pText);

	//Write the requested data
	eResult = pTerminal->pIOObj->pfWriteData(pTerminal->pIOObj, (void *)pText, nTextLen);
	if (eResult < Success) {
		return eResult;
	}

	//Add end of line characters
	eResult = pTerminal->pIOObj->pfWriteData(pTerminal->pIOObj, (void *)"\r\n", 2);

	return eResult;
}

eReturn_t TerminalReadInput(sTerminal_t *pTerminal) {
	eReturn_t eResult;
	uint32_t nReadBytes, nCtr;

	//Read data from the IOConnect to fill the buffer
	eResult = pTerminal->pIOObj->pfReadData(pTerminal->pIOObj, (void *)&(pTerminal->aInputBuffer[pTerminal->nBufferUsed]), TERMINAL_BUFFERSIZE - pTerminal->nBufferUsed, &nReadBytes);

	if (eResult < Success) {
		return Fail_CommError;
	}

	pTerminal->nBufferUsed += nReadBytes; //Update how much of the buffer is used

	if (nReadBytes > 0) {
		TerminalWriteTextLine(pTerminal, "Found text!");
	}

	//Look through the buffer for new line characters
	for (nCtr = 0; nCtr < TERMINAL_BUFFERSIZE; nCtr++) {

	}

	return Success;
}
